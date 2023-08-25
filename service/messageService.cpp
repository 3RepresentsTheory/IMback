//
// Created by no3core on 2023/8/24.
//

#include "messageService.h"
#include "../HttpServerHeaders/utils.h"
#include "../Dao/DataClasses.h"

MessageService::~MessageService() {
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
    string sql = "SELECT * FROM message WHERE id = ?";
    vector<map<string, string>>
        ret = baseDao->executeQuery(sql,to_string(original_message.id));
    // database must guarantee that only one message of this id
    original_message.type    = QString::fromStdString(ret[0]["type"]);
    original_message.time    = getUnixTimeStampFromString(ret[0]["time"]);
    original_message.mid     = std::stoi(ret[0]["mid"]);
}

QVector<Message> MessageService::GetMessagelistByTime(qint64 mid) {
    string sql = "SELECT * FROM message WHERE id > ?";
    vector<map<string,string>>
        ret = baseDao->executeQuery(sql,to_string(mid));
    auto MessageList = QVector<Message>();
    for(auto row : ret){
        MessageList.emplace_back(Message(row));
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

