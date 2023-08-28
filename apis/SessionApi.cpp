//
// Created by no3core on 2023/8/24.
//
#include "SessionApi.h"
//TODO: eliminate dup session tokens

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
    lock2.lockForWrite();
    lock3.lockForWrite();
    id2ip.insert(make_pair(id, ip));
    idSets.insert(id);
    lock3.unlock();
    lock2.unlock();
}

std::optional<string> SessionApi::getIpById(string id) {
    lock2.lockForRead();
    auto it = id2ip.find(id);
    if (it != id2ip.end()) {
        // 键存在，返回对应的值
        lock2.unlock();
        return it->second;
    } else {
        lock2.unlock();
        // 键不存在，返回空字符串
        return std::nullopt;
    }
}


QJsonArray SessionApi:: checkIdsInSet(const string& uid) {
    QJsonArray result;
    QStringList idList = QString::fromStdString(uid).split(',');

    lock3.lockForRead();
    for (const QString& id : idList) {
        result.append(idSets.count(id.toStdString()) > 0);
    }
    lock3.unlock();
    return result;
}

void SessionApi::removeUser(const string &id) {

    lock2.lockForRead();
    lock3.lockForRead();
    auto it = idSets.find(id);  // 查找要移除的元素
    if (it != idSets.end()) {
        idSets.erase(it);  // 从 set 中移除元素
    }

    auto it2 = id2ip.find(id);  // 查找要移除的键
    if (it2 != id2ip.end()) {
        id2ip.erase(it2);  // 从 map 中移除键值对
    }
    lock3.unlock();
    lock2.unlock();
}


SessionApi::SessionApi() {

}

SessionApi::~SessionApi() {

}

