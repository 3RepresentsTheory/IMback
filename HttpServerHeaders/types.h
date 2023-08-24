// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifndef TYPES_H
#define TYPES_H

#include <QtGui/QColor>
#include <QtCore/QDateTime>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QString>
#include <QtCore/qtypes.h>
#include <string>
#include <algorithm>
#include <optional>

using namespace std;

template<typename T>
struct FromJsonFactory
{
    virtual T* fromJson(const QJsonObject &json) const = 0;
    virtual ~FromJsonFactory() = default;
};

struct SessionEntry
{
    qint64 id;

    string username;
    string password;
    string nickname;

    std::optional<QUuid> token;

    explicit SessionEntry(const string &username, const string &password, const string &nickname)
            : username(username), password(password), nickname(nickname){}

    void startSession() { token = generateToken(); }
    void endSession()   { token = std::nullopt   ; }

    QJsonObject registerJson()
    {
        return token
               ? QJsonObject{
                              { "token", token->toString(QUuid::StringFormat::WithoutBraces) }
                }
               : QJsonObject{};
    }

    QJsonObject loginJson()
    {
        QJsonObject jsonObject;

        if (token)
        {
            jsonObject["nickname"] = QString::fromStdString(nickname);
            jsonObject["token"]    = token->toString(QUuid::StringFormat::WithoutBraces);
        }

        return jsonObject;
    }


    bool operator==(const QString &otherToken) const
    {
        return token && *token == QUuid::fromString(otherToken);
    }

private:
    QUuid generateToken() {
        return QUuid::createUuid();
    }
};



struct SessionEntryFactory : public FromJsonFactory<SessionEntry>
{
    SessionEntry* fromJson(const QJsonObject &json) const override
    {
        if (
           !json.contains("username")||
           !json.contains("password")||
           !json.contains("nickname")||
           json.value("username").isNull()||
           json.value("password").isNull()||
           json.value("nickname").isNull()
        )
            return nullptr;
        return new
            SessionEntry(
                    json.value("username").toString().toStdString(),
                    json.value("password").toString().toStdString(),
                    json.value("nickname").toString().toStdString()
            );
    }
};




template<typename T>
class tokenMap : public QMap<qint64, T>
{
public:
    tokenMap() = default;
    explicit tokenMap(const FromJsonFactory<T> &factory, const QJsonArray &array) : QMap<qint64, T>()
    {
        for (const auto &jsonValue : array) {
            if (jsonValue.isObject()) {
                const auto maybeT = factory.fromJson(jsonValue.toObject());
                if (maybeT) {
                    QMap<qint64, T>::insert(maybeT->token, *maybeT);
                }
            }
        }
    }
};


#endif // TYPES_H
