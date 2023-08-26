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


bool HistoryRqst::fromQJsonObject(const QJsonObject &json) {
    if(
            !json.contains("gid")     || json.value("gid").isNull()||
            !json.contains("last")  || json.value("last").isNull()
    )
        return false;
    gid = json.value("gid").toInteger();
    mid = json.value("last").toInteger();
}

QJsonObject HistoryRqst::toQJsonObject() {
    return QJsonObject{
            {"gid",gid},
            {"last",mid},
    };
}