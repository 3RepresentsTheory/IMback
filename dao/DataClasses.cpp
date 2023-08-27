//
// Created by no3core on 2023/8/25.
//
#include "DataClasses.h"

bool Message::fromQJsonObject(const QJsonObject & json) {
    if(
            !json.contains("type")     || json.value("type").isNull()||
            !json.contains("content")  || json.value("content").isNull()||
            !json.contains("gid")      || json.value("gid").isNull()
            )
        return false;

    type    = json.value("type").toString();
    content = json.value("content").toString();
    gid     = json.value("gid").toInt();

    return true;
}

QJsonObject Message::toQJsonObject() {
    return QJsonObject{
            {"id",      QString::fromStdString(to_string(id))},
            {"type",    type   },
            {"content", content},
            {"time",    time},
            {"uid",     uid},
            {"mid",     QString::fromStdString(to_string(mid))},
            {"gid",     gid}
    };
}

Message::Message(dao_entry row,bool isFromGroupCstr) {
    if(!isFromGroupCstr){
        id      = std::stoi(row["id"]);
        type    = QString::fromStdString(row["type"]);
        content = QString::fromStdString(row["content"]);
        time    = std::stoi(row["time"]);
        uid     = std::stoi(row["uid"]);
        mid     = std::stoi(row["mid"]);
        gid     = std::stoi(row["gid"]);
    }else{
        id      = std::stoi(row["m.id"]);
        type    = QString::fromStdString(row["m.type"]);
        content = QString::fromStdString(row["content"]);
        time    = std::stoi(row["time"]);
        uid     = std::stoi(row["uid"]);
        mid     = std::stoi(row["mid"]);
        gid     = std::stoi(row["gid"]);
    }
}


bool HistoryRqst::fromQJsonObject(const QJsonObject &json) {
    if(
            !json.contains("gid")     || json.value("gid").isNull()||
            !json.contains("last")  || json.value("last").isNull()
    )
        return false;
    gid = json.value("gid").toInteger();
    mid = json.value("last").toInteger();
    return true;
}

QJsonObject HistoryRqst::toQJsonObject() {
    return QJsonObject{
            {"gid",gid},
            {"last",mid},
    };
}

Group::Group(dao_entry row,bool isContainLastMessage) {
    if(!isContainLastMessage){
        id      = QString::fromStdString(row["id"]);
        name    = QString::fromStdString(row["name"]);
        owner   = std::stoi(row["owner"]);
        type    = QString::fromStdString(row["type"]);
    } else{
        id      = QString::fromStdString(row["gc.id"]);
        name    = QString::fromStdString(row["name"]);
        owner   = std::stoi(row["owner"]);
        type    = QString::fromStdString(row["gc.type"]);
        last_message = Message(row, true);
    }

}

bool Group::fromQJsonObject(const QJsonObject &json) {
    if(
            !json.contains("gname")     || json.value("gname").isNull())
        return false;

    name    = json.value("gname").toString();
    return true;
}

QJsonObject Group::toQJsonObject() {
    return QJsonObject{
            {"id",      id},
            {"name",    name},
            {"type",    type},
            {"owner",   owner},
            {"last", QJsonValue::Null},
    };
}

QJsonObject Group::toQJsonObjectWithLastMsg() {
    return QJsonObject{
            {"id",           id},
            {"name",         name},
            {"type",         type},
            {"owner",        owner},
            {"last", last_message.toQJsonObject()},
    };
}
