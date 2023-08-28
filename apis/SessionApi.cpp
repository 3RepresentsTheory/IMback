//
// Created by no3core on 2023/8/24.
//
#include "SessionApi.h"

SessionEntry SessionApi::createEntryAndStart(qint64 id) {
    SessionEntry sessionEntry;
    sessionEntry.startSession();
    sessionEntry.id = id;
    lock.lockForWrite();
    sessions.insertEntry(sessionEntry);
    lock.unlock();
    return sessionEntry;
}

int SessionApi::addtoSessionlist(SessionEntry *entry) {
    return 0;
}

int SessionApi::removeEntry(SessionEntry *target) {
    return 0;
}

std::optional<int> SessionApi::getIdByCookie(QUuid token) {
    lock.lockForRead();
    SessionEntry sessionEntry = sessions.value(token);
    lock.unlock();
    if(sessionEntry.token != token){
        return std::nullopt;
    }
    return sessionEntry.id;
}

void SessionApi::insertIP(const string &id, const string &ip) {
    id2ip.insert(make_pair(id, ip));
    idSets.insert(id);
}

std::optional<string> SessionApi::getIpById(string id) {
    auto it = id2ip.find(id);
    if (it != id2ip.end()) {
        // 键存在，返回对应的值
        return it->second;
    } else {
        // 键不存在，返回空字符串
        return std::nullopt;
    }
}


QJsonArray SessionApi:: checkIdsInSet(const string& uid) {
    QJsonArray result;
    QStringList idList = QString::fromStdString(uid).split(',');

    for (const QString& id : idList) {
        result.append(idSets.count(id.toStdString()) > 0);
    }

    return result;
}

SessionApi::SessionApi() {

}

SessionApi::~SessionApi() {

}
