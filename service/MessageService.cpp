//
// Created by no3core on 2023/8/24.
//

#include "MessageService.h"

MessageService::MessageService() {
    baseDao = new BaseDao();
}

MessageService::~MessageService() {
    //TODO: might cause double free? had better use like this:
    /* auto service = MessageService(
     *         new BaseDao("E:\\programming\\Qt\\IMback\\test\\dbfortest.db")
     * );
     */
    if(baseDao!= nullptr)
        delete(baseDao);
}

bool MessageService::StoreMessage(const Message &message,int &last_insert_id) {
    string sql = "INSERT INTO message(type, content, gid, uid) VALUES (?, ?, ?, ?);";
    return baseDao->executeUpdate(
        last_insert_id,
        sql,
        message.type.toStdString(),
        message.content.toStdString(),
        to_string(message.gid),
        to_string(message.uid)
    );
}

void MessageService::FillMessageFromDB(Message &original_message) {
    // have to make sure id has been filled outside
    string sql = "SELECT * FROM message WHERE id = ?";
    vector<map<string, string>>
        ret = baseDao->executeQuery(sql,to_string(original_message.id));
    // database must guarantee that only one message of this id
    original_message.type    = QString::fromStdString(ret[0]["type"]);
    original_message.time    = getUnixTimeStampFromString(ret[0]["time"]);
    original_message.mid     = std::stoi(ret[0]["mid"]);
}

QJsonArray MessageService::GetMessagelistByTime(qint64 mid, qint64 gid) {
    string sql = "SELECT * FROM message WHERE id > ? and gid = ?";
    vector<map<string,string>>
        ret = baseDao->executeQuery(sql,to_string(mid),to_string(gid));
    auto MessageList = QJsonArray();
    for(auto row : ret){
        MessageList.append(Message(row).toQJsonObject());
    }
    return MessageList;
}

QVector<qint64> MessageService::GetGroupUserList(qint64 gid) {
    string sql = "SELECT * FROM groupUser WHERE gid = ?";
    vector<map<string,string>>
            ret = baseDao->executeQuery(sql,to_string(gid));
    auto userList = QVector<qint64>();
    for(auto row : ret){
        userList.emplace_back(std::stoi(row["uid"]));
    }
    return userList;
}

