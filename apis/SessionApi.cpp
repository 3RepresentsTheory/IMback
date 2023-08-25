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
    //TODO:add logout
    return 0;
}

std::optional<int> SessionApi::getIdByCookie(QUuid token) {
    SessionEntry sessionEntry = sessions.value(token);
    if(sessionEntry.token != token){
        return std::nullopt;
    }
    return sessionEntry.id;
}
