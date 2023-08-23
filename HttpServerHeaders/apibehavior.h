// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifndef APIBEHAVIOR_H
#define APIBEHAVIOR_H

#include "types.h"
#include "utils.h"
#include "../Dao/userDao.h"
#include <QtHttpServer/QHttpServer>
#include <QtConcurrent/qtconcurrentrun.h>
#include <string>

#include <optional>

class SessionApi
{
public:
    explicit SessionApi(const IdMap<SessionEntry> &sessions,
                        std::unique_ptr<FromJsonFactory<SessionEntry>> factory)
    : sessions(sessions), factory(std::move(factory)),userDao(new UserDao())
    {
    }

    QHttpServerResponse registerSession(const QHttpServerRequest &request)
    {
        const auto json = byteArrayToJsonObject(request.body());
        if (!json)
            return QHttpServerResponse("接收消息失败或为空",QHttpServerResponder::StatusCode::BadRequest);
        SessionEntry* sessionEntry = factory->fromJson(*json);
        if (!sessionEntry)
            return QHttpServerResponse("请输入完整的username,password,nickname,且不能为空",QHttpServerResponder::StatusCode::BadRequest);
        if(userDao->isUsernameExists(sessionEntry->username.toStdString())){
            return QHttpServerResponse("用户名已存在，请重试",QHttpServerResponder::StatusCode::BadRequest);
        }
        if(userDao->isNicknameExists(sessionEntry->nickname.toStdString())){
            return QHttpServerResponse("昵称已存在，请重试",QHttpServerResponder::StatusCode::BadRequest);
        }
        userDao->insertUser(sessionEntry->username.toStdString(),sessionEntry->password.toStdString(),sessionEntry->nickname.toStdString());
        sessionEntry->id = userDao->selectIdByName(sessionEntry->username.toStdString());
        sessionEntry->startSession();
        return QHttpServerResponse(sessionEntry->toJson());
    }

    QHttpServerResponse login(const QHttpServerRequest &request)
    {
        const auto json = byteArrayToJsonObject(request.body());
        if (!json)
            return QHttpServerResponse("接收消息失败或为空",QHttpServerResponder::StatusCode::BadRequest);
        if (!json->contains("username") || !json->contains("password")||
        !json->value("username").isNull()||!json->value("password").isNull())
            return QHttpServerResponse("请输入完整的username,password,且不能为空",QHttpServerResponder::StatusCode::BadRequest);

        UserCredentials userCredentials = userDao->validateUserCredentials(json->value("username").toString().toStdString(),json->value("password").toString().toStdString());
        if(!get<0>(userCredentials)){
            QHttpServerResponse("用户名或密码错误",QHttpServerResponder::StatusCode::BadRequest);
        } else{
            new SessionEntry(get<1>(userCredentials))
        }
        auto maybeSession = std::find_if(
                sessions.begin(), sessions.end(),
                [email = json->value("username").toString(),
                        password = json->value("password").toString()](const auto &it) {
                    return it.password == password && it.username == email;
                });
        if (maybeSession == sessions.end()) {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
        }

        maybeSession->startSession();
        return QHttpServerResponse(maybeSession->toJson());
    }

    QHttpServerResponse logout(const QHttpServerRequest &request)
    {
        const auto maybeToken = getTokenFromRequest(request);
        if (!maybeToken)
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);

        auto maybeSession = std::find(sessions.begin(), sessions.end(), *maybeToken);
        if (maybeSession != sessions.end())
            maybeSession->endSession();
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Ok);
    }

    bool authorize(const QHttpServerRequest &request) const
    {
        const auto maybeToken = getTokenFromRequest(request);
        if (maybeToken) {
            const auto maybeSession = std::find(sessions.begin(), sessions.end(), *maybeToken);
            return maybeSession != sessions.end() && *maybeSession == *maybeToken;
        }
        return false;
    }

private:
    IdMap<SessionEntry> sessions;
    std::unique_ptr<FromJsonFactory<SessionEntry>> factory;
    UserDao *userDao;
};

#endif // APIBEHAVIOR_H
