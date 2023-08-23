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

#include <algorithm>
#include <optional>

struct Jsonable
{
    virtual QJsonObject toJson() const = 0;
    virtual ~Jsonable() = default;
};

template<typename T>
struct FromJsonFactory
{
    virtual T* fromJson(const QJsonObject &json) const = 0;
    virtual ~FromJsonFactory() = default;
};

struct SessionEntry : public Jsonable
{
    qint64 id;
    QString username;
    QString password;
    QString nickname;
    std::optional<QUuid> token;

    explicit SessionEntry(const QString &username, const QString &password, const QString &nickname)
            : username(username), password(password), nickname(nickname)
    {
    }

    void startSession() { token = generateToken(); }

    void endSession() { token = std::nullopt; }

    QJsonObject toJson() const override
    {
        return token
               ? QJsonObject{ { "id", id },
                              { "token", token->toString(QUuid::StringFormat::WithoutBraces) } }
               : QJsonObject{};
    }

    bool operator==(const QString &otherToken) const
    {
        return token && *token == QUuid::fromString(otherToken);
    }

private:
    QUuid generateToken() { return QUuid::createUuid(); }

    static qint64 nextId()
    {
        static qint64 lastId = 1;
        return lastId++;
    }
};

struct SessionEntryFactory : public FromJsonFactory<SessionEntry>
{
    SessionEntry* fromJson(const QJsonObject &json) const override
    {
        if (!json.contains("username") || !json.contains("password") ||!json.contains("nickname")
        ||json.value("username").isNull()||json.value("password").isNull()||json.value("nickname").isNull())
            return nullptr;
        return new SessionEntry(json.value("username").toString(), json.value("password").toString(),json.value("nickname").toString());
    }
};

template<typename T>
class IdMap : public QMap<qint64, T>
{
public:
    IdMap() = default;
    explicit IdMap(const FromJsonFactory<T> &factory, const QJsonArray &array) : QMap<qint64, T>()
    {
        for (const auto &jsonValue : array) {
            if (jsonValue.isObject()) {
                const auto maybeT = factory.fromJson(jsonValue.toObject());
                if (maybeT) {
                    QMap<qint64, T>::insert(maybeT->id, *maybeT);
                }
            }
        }
    }
};


#endif // TYPES_H
