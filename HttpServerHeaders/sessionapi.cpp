//
// Created by no3core on 2023/8/24.
//
#include "sessionapi.h"

SessionEntry *SessionApi::createEntryAndStart(qint64 id) {
    SessionEntry *sessionEntry = new SessionEntry();
    sessionEntry->startSession();
    sessionEntry->id = id;
    sessions.insertEntry(sessionEntry);
    return nullptr;
}

int SessionApi::addtoSessionlist(SessionEntry *entry) {
    return 0;
}

int SessionApi::removeEntry(SessionEntry *target) {
    return 0;
}
