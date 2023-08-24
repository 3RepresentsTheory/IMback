// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifndef UTILS_H
#define UTILS_H

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtHttpServer/QtHttpServer>

#include <optional>
#include "types.h"

static std::optional<QByteArray> readFileToByteArray(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::nullopt;

    return file.readAll();
}

static std::optional<QJsonArray> byteArrayToJsonArray(const QByteArray &arr)
{
    QJsonParseError err;
    const auto json = QJsonDocument::fromJson(arr, &err);
    if (err.error || !json.isArray())
        return std::nullopt;
    return json.array();
}

static std::optional<QJsonObject> byteArrayToJsonObject(const QByteArray &arr)
{
    QJsonParseError err;
    const auto json = QJsonDocument::fromJson(arr, &err);
    if (err.error || !json.isObject())
        return std::nullopt;
    return json.object();
}

static QByteArray getValueFromHeader(const QList<QPair<QByteArray, QByteArray>> &headers,
                                     const QString &keyToFind)
{
    for (const auto &[key, value] : headers) {
        if (key == keyToFind) {
            return value;
        }
    }
    return QByteArray();
}

static std::optional<QString> getcookieFromRequest(const QHttpServerRequest &request)
{
    std::optional<QString> cookie;
    if (auto bytes = getValueFromHeader(request.headers(), "cookie"); !bytes.isEmpty()) {
        cookie = bytes;
    }
    return cookie;
}



#endif // UTILS_H
