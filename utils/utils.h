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

static std::optional<QByteArray> readFileToByteArray(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::nullopt;

    return file.readAll();
}

static std::optional<QJsonArray> byteArrayToJsonArray(const QByteArray &arr) {
    QJsonParseError err;
    const auto json = QJsonDocument::fromJson(arr, &err);
    if (err.error || !json.isArray())
        return std::nullopt;
    return json.array();
}

static std::optional<QJsonObject> byteArrayToJsonObject(const QByteArray &arr) {
    QJsonParseError err;
    const auto json = QJsonDocument::fromJson(arr, &err);
    if (err.error || !json.isObject())
        return std::nullopt;
    return json.object();
}

static QByteArray getValueFromHeader(const QList<QPair<QByteArray, QByteArray>> &headers,
                                     const QString &keyToFind) {
    for (const auto &[key, value]: headers) {
        if (key == keyToFind) {
            return value;
        }
    }
    return QByteArray();
}

static std::optional<QString> getcookieFromRequest(const QHttpServerRequest &request) {
    std::optional<QString> cookie;
    if (auto bytes = getValueFromHeader(request.headers(), "Cookie"); !bytes.isEmpty()) {
        cookie = bytes;
    }
    return cookie;
}


inline map<string, string> jsonToString(const QJsonObject jsonObject) {
    map<string, string> result;


    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        QString key = it.key();
        QJsonValue value = it.value();

        if (value.isString()) {
            QString stringValue = value.toString();
            string stdValue = stringValue.toStdString();
            result[key.toStdString()] = stdValue;
        } else if (value.isDouble()) {
            int doubleValue = value.toInt();
            string stdValue = to_string(doubleValue);
            result[key.toStdString()] = stdValue;
        } else if (value.isBool()) {
            bool boolValue = value.toBool();
            string stdValue = boolValue ? "true" : "false";
            result[key.toStdString()] = stdValue;
        }
    }

    return result;
}

static std::time_t getUnixTimeStampFromString(const string&timestring){
    std::tm t{};
    std::istringstream ss(timestring);
    ss>>std::get_time(&t,"%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        throw std::runtime_error{"failed to parse time string"};
    }
    return mktime(&t);
}
static QString prettyIPV4(const QHostAddress&qhostaddr){
    return QHostAddress(qhostaddr.toIPv4Address()).toString();
}

static QString mappingMethod(QHttpServerRequest::Method method){

    switch (method) {
        case QHttpServerRequest::Method::Unknown:
            return "Unknown";
        case QHttpServerRequest::Method::Get:
 return "Get ";
        case QHttpServerRequest::Method::Put:
 return "Put";
        case QHttpServerRequest::Method::Delete:
 return "Delete";
        case QHttpServerRequest::Method::Post:
 return "Post";
        case QHttpServerRequest::Method::Head:
 return "Head";
        case QHttpServerRequest::Method::Options:
 return "Options";
        case QHttpServerRequest::Method::Patch:
 return "Patch";
        case QHttpServerRequest::Method::Connect:
 return "Connect";
        case QHttpServerRequest::Method::Trace:
 return "Trace";
        case QHttpServerRequest::Method::AnyKnown:
 return "AnyKnown";
    }
};

#define RGB_STRING_PROLOGUE(a,b,c) "\x1b[38;2;" a ";" b ";" c ";m"
#define COLOR_EPILOGUE "\x1b[0m"

static QString LogHeader(const QHttpServerRequest &request){
    QString log;
    log.append(prettyIPV4(request.remoteAddress()));
    log.append(":");
    log.append(QString::number(request.remotePort()));
    log.append(" ");
    log.append(mappingMethod(request.method()));
    log.append(" ");
    log.append(request.url().toString());
    log.append(" ");
    return log;
}







#endif // UTILS_H
