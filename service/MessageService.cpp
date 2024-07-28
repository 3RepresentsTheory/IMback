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
    string sql = "INSERT INTO message(type, content, uid, gid) VALUES (?, ?, ?, ?);";
    return baseDao->executeUpdate(
        last_insert_id,
        sql,
        message.type.toStdString(),
        message.content.toStdString(),
        to_string(message.uid),
        to_string(message.gid)
    );
}

void MessageService::FillMessageFromDB(Message &original_message) {
    // have to make sure id has been filled outside
    string sql = "SELECT * FROM message WHERE id = ?";
    vector<map<string, string>>
        ret = baseDao->executeQuery(sql,to_string(original_message.id));
    // database must guarantee that only one message of this id
    original_message.type    = QString::fromStdString(ret[0]["type"]);
    original_message.time    = std::stoi(ret[0]["time"]);
    original_message.mid     = std::stoi(ret[0]["mid"]);
}

QJsonArray MessageService::GetMessagelistByPeriod(qint64 gid, qint64 start, qint64 end) {
    string sql = "SELECT * FROM message WHERE mid BETWEEN ? and ? and gid = ?";
    vector<map<string,string>>
        ret = baseDao->executeQuery(sql,to_string(start),to_string(end),to_string(gid));
    auto MessageList = QJsonArray();
    for(auto row : ret){
        MessageList.append(Message(row).toQJsonObject());
    }
    return MessageList;
}

QVector<qint64> MessageService::GetGroupUserList(qint64 gid) {
    // if gid is not valid, return empty vector
    string sql = "SELECT * FROM groupUser WHERE gid = ?";
    vector<map<string,string>>
            ret = baseDao->executeQuery(sql,to_string(gid));
    auto userList = QVector<qint64>();
    for(auto row : ret){
        userList.emplace_back(std::stoi(row["uid"]));
    }
    return userList;
}

