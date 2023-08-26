// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifndef APIBEHAVIOR_H
#define APIBEHAVIOR_H

#include "../Utils/types.h"
#include "../Utils/utils.h"
#include <QtHttpServer/QHttpServer>
#include <QtConcurrent/qtconcurrentrun.h>
#include <string>
#include "../service/UserService.h"
#include <optional>

class SessionApi {
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

    int addtoSessionlist(SessionEntry* entry);

    int removeEntry(SessionEntry * target);


private:
    tokenMap sessions;
    QReadWriteLock lock;
};

#endif // APIBEHAVIOR_H
