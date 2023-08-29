// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifndef APIBEHAVIOR_H
#define APIBEHAVIOR_H

#include "../Utils/types.h"
#include "../Utils/utils.h"
#include <QtHttpServer/QHttpServer>
#include <QtConcurrent/qtconcurrentrun.h>
#include <string>
#include <optional>
#include <set>

class SessionApi :public QObject{
    Q_OBJECT
public:
    SessionApi();
    ~SessionApi();

    static SessionApi* getInstance() {
        static SessionApi instance;
        return &instance;
    }

    SessionApi(const SessionApi&) = delete;
    SessionApi& operator=(const SessionApi&) = delete;

    SessionEntry createEntryAndStart(qint64 id);

    std::optional<int> getIdByCookie(QUuid token);

    void insertIP(const string& id,const string& ip);

    std::optional<string> getIpById(string id);

    int addtoSessionlist(SessionEntry* entry);

    int removeEntry(SessionEntry * target);

    void removeUser(const string& id);

    QJsonArray checkIdsInSet(const string& uid);

public slots:
    void onUserLogout(qint64 uid);
    void onUserLogin(QString uid,QString ip);

private:
    tokenMap sessions;
    map<string,string> id2ip;
    QReadWriteLock lock;
    QReadWriteLock lock2;
    QReadWriteLock lock3;
    set<string> idSets;
};

#endif // APIBEHAVIOR_H
