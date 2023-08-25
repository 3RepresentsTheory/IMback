//
// Created by no3core on 2023/8/24.
//
#include "SessionApi.h"

SessionEntry SessionApi::createEntryAndStart(qint64 id) {
    SessionEntry sessionEntry;
    sessionEntry.startSession();
    sessionEntry.id = id;
    sessions.insertEntry(sessionEntry);
    return sessionEntry;
}

int SessionApi::addtoSessionlist(SessionEntry *entry) {
    return 0;
}

int SessionApi::removeEntry(SessionEntry *target) {
    return 0;
}

int SessionApi::authcookie(QUuid token) {
    SessionEntry sessionEntry = sessions.value(token);
    if(sessionEntry.token != token){
        return -1;
    }
    return sessionEntry.id;
}
