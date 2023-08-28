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
                {"id",      id},
                {"type",    type   },
                {"content", content},
                {"time",    time},
                {"uid",     uid},
                {"mid",     mid},
                {"gid",     gid}
        };
}

Message::Message(dao_entry row,bool isFromGroupCstr) {
    id      = std::stoi(row["id"]);
    if(id==0)
        //this is a special pseudo msg
        return;
    type    = QString::fromStdString(row["type"]);
    content = QString::fromStdString(row["content"]);
    time    = std::stoi(row["time"]);
    uid     = std::stoi(row["uid"]);
    mid     = std::stoi(row["mid"]);
    gid     = std::stoi(row["gid"]);
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
    id      = std::stoi(row["gcid"]);
    name    = QString::fromStdString(row["name"]);
    owner   = std::stoi(row["owner"]);
    type    = QString::fromStdString(row["gctype"]);
    if(isContainLastMessage)
        last_message = Message(row, true);
    avatar  = QString::fromStdString(row["avatar"]);
    color   = QString::fromStdString(row["color"]);
}

bool Group::fromQJsonObject(const QJsonObject &json) {
    if(
            !json.contains("name")     || json.value("name").isNull()||
            !json.contains("avatar")   || json.value("avatar").isNull()||
            !json.contains("color")    || json.value("color").isNull())
        return false;

    name    = json.value("name").toString();
    avatar  = json.value("avatar").toString();
    color   = json.value("color").toString();
    return true;
}

QJsonObject Group::toQJsonObject() {
    return QJsonObject{
            {"id",      id},
            {"name",    name},
            {"type",    type},
            {"owner",   owner},
            {"color",   color},
            {"avatar",  avatar},
            {"last", QJsonValue::Null},
    };
}

QJsonObject Group::toQJsonObjectWithLastMsg() {
    if(last_message.id==0)
        return QJsonObject{
                {"id",      id},
                {"name",    name},
                {"type",    type},
                {"owner",   owner},
                {"color",   color},
                {"avatar",  avatar},
                {"last", QJsonValue::Null},
        };
    else
    return QJsonObject{
            {"id",           id},
            {"name",         name},
            {"type",         type},
            {"owner",        owner},
            {"color",   color},
            {"avatar",  avatar},
            {"last", last_message.toQJsonObject()},
    };
}
