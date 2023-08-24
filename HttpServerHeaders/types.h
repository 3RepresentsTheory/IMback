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

struct SessionEntry
{
    qint64 id;
    std::optional<QUuid> token;

    explicit SessionEntry(){}

    void startSession() { token = generateToken(); }
    void endSession()   { token = std::nullopt   ; }

    bool operator==(const QString &otherToken) const
    {
        return token && *token == QUuid::fromString(otherToken);
    }

private:
    QUuid generateToken() {
        return QUuid::createUuid();
    }
};



class tokenMap : public QMap<QUuid, SessionEntry*>
{
public:
    tokenMap() = default;
    // have to guarantee that token is vaild
    int insertEntry(SessionEntry* maybeT){
        if(maybeT&&maybeT->token.has_value()){
            QMap<QUuid,SessionEntry*>::insert(maybeT->token.value(),maybeT);
            return 0;
        }else
            return -1;
    }


};


class idMap : public QMap<SessionEntry*, QUuid>
{
public:
    idMap() = default;
    // have to guarantee that token is vaild
    int insertEntry(SessionEntry* maybeT){
        if(maybeT&&maybeT->token.has_value()){
            QMap<SessionEntry*,QUuid>::insert(maybeT,maybeT->token.value());
            return 0;
        }else
            return -1;
    }


};


#endif // TYPES_H
