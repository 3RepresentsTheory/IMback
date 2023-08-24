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
#include "../service/userService.h"
#include <optional>

class SessionApi {
public:
    explicit SessionApi(
            unique_ptr<FromJsonFactory<SessionEntry>> factory
    )
    :factory(std::move(factory)), userService(new UserService()) {
    }

    QHttpServerResponse registerSession(const QHttpServerRequest &request) {
        const auto json = byteArrayToJsonObject(request.body());

        if (!json)
            return QHttpServerResponse("接收消息失败或为空", QHttpServerResponder::StatusCode::BadRequest);

        SessionEntry *sessionEntry = factory->fromJson(*json);
        if (!sessionEntry)
            return QHttpServerResponse("请输入完整的username,password,nickname,且不能为空",
                                       QHttpServerResponder::StatusCode::BadRequest);

        if (userService->isUsernameExists(sessionEntry->username)) {
            return QHttpServerResponse("用户名已存在，请重试", QHttpServerResponder::StatusCode::BadRequest);
        }

        if (userService->isNicknameExists(sessionEntry->nickname)) {
            return QHttpServerResponse("昵称已存在，请重试", QHttpServerResponder::StatusCode::BadRequest);
        }

        userService->insertUser(sessionEntry->username, sessionEntry->password, sessionEntry->nickname);

        sessionEntry->id = userService->selectIdByName(sessionEntry->username);
        sessionEntry->startSession();

        return QHttpServerResponse(sessionEntry->registerJson());
    }

    // !!wait to fix
    QHttpServerResponse login(const QHttpServerRequest &request) {
        const auto json = byteArrayToJsonObject(request.body());

        if (!json)
            return QHttpServerResponse("接收消息失败或为空", QHttpServerResponder::StatusCode::BadRequest);

        if (
           !json->contains("username") ||
           !json->contains("password") ||
           json->value("username").toString().toStdString().empty() ||
           json->value("password").toString().toStdString().empty()
        ) {
            return QHttpServerResponse(
                    "请输入完整的username,password,且不能为空",
                    QHttpServerResponder::StatusCode::BadRequest
            );
        }

        string nickname =
                userService->validateUserCredentials(
                    json->value("username").toString().toStdString(),
                    json->value("password").toString().toStdString()
        );

        SessionEntry *sessionEntry = nullptr;

        if (nickname.length()==0) {
            return QHttpServerResponse("用户名或密码错误", QHttpServerResponder::StatusCode::BadRequest);
        } else {
            sessionEntry = new SessionEntry(
                    json->value("username").toString().toStdString(),
                    json->value("password").toString().toStdString(),
                    nickname
            );
        }
        sessionEntry->startSession();
        return QHttpServerResponse(sessionEntry->loginJson());
    }

    // !!wait to fix
    QHttpServerResponse logout(const QHttpServerRequest &request) {
        const auto maybecookie = getcookieFromRequest(request);
        if (!maybecookie)
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);

        auto maybeSession = std::find(sessions.begin(), sessions.end(), *maybecookie);
        if (maybeSession != sessions.end())
            maybeSession->endSession();
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Ok);
    }

private:
    tokenMap<SessionEntry> sessions;
    unique_ptr<FromJsonFactory<SessionEntry>> factory;
    UserService *userService;
};

#endif // APIBEHAVIOR_H
