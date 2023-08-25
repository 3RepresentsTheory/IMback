// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifndef APIBEHAVIOR_H
#define APIBEHAVIOR_H

#include "../Utils/types.h"
#include "../Utils/utils.h"
#include "../Dao/UserDao.h"
#include <QtHttpServer/QHttpServer>
#include <QtConcurrent/qtconcurrentrun.h>
#include <string>
#include "../service/UserService.h"
#include <optional>

class SessionApi {
public:
    explicit SessionApi() {}

    SessionEntry createEntryAndStart(qint64 id);

    int authcookie(QUuid token);

    int addtoSessionlist(SessionEntry* entry);

    int removeEntry(SessionEntry * target);


private:
    tokenMap sessions;
};

#endif // APIBEHAVIOR_H
