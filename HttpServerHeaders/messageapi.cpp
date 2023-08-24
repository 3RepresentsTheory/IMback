//
// Created by no3core on 2023/8/24.
//

#include "messageapi.h"
#include "utils.h"


bool Message::fromQJsonObject(const QJsonObject & json) {
    if(
        !json.contains("type")     || json.value("type").isNull()||
        !json.contains("content")  || json.value("content").isNull()||
        !json.contains("gid")      || json.value("gid").isNull()
    )
        return false;

    type = json.value("type").toString();
    content = json.value("content").toString();
    gid  = json.value("gid").toInt();

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

QHttpServerResponse MessageApi::handleSentMessageRequest(const QHttpServerRequest &request) {
    Message message;
    // json convert validating
    const auto json = byteArrayToJsonObject(request.body());

    if (!json)
        return QHttpServerResponse("接收消息失败或为空", QHttpServerResponder::StatusCode::BadRequest);

    // get the cookie and auth it, fail then return
    if(
            !getcookieFromRequest(request).has_value()
            // ||session.authcookie(getcookieFromRequest(request).value())
            //TODO:add session auth cookie
    )
        return QHttpServerResponse("Authentication failed", QHttpServerResponder::StatusCode::BadRequest);

    // json field validating make sure gid, type ,content is not blank
    if(message.fromQJsonObject(json.value())<0){
        return QHttpServerResponse("接收消息失败或为空", QHttpServerResponder::StatusCode::BadRequest);
    }

    // generate complete database object(store to database ,and have a copy in memory)
    // including:
        /*
         * type         (fill in)
         * content      (fill in)
         * gid          (fill in)
         * id           (generate - database)
         * time         (generate - database)
         * mid          (generate - database)
         * uid          (generate - map cookie to id, use sessionapi needed)
         */
    // we simply fill type content gid uid(use session api) ,store to database and read it

    // ready to broadcast Message to group
    // broadcastMessageToGroup(gid(fill in), content(fill in))

    // check broadcast if success return jsonfy object, or fail simply return 400
}

int MessageApi::broadcastMessageToGroup(int gid, QString messagetext) {
    // access database to generate a broadcast list (gid -> [uid1,uid2...])
    // here we ignore if someone add to this group and he miss it
    // no cache for this, simply read from database

    // call the chatserver to broadcast using some method (we assume this method is
    // atomically -- its will all send/or not send at all)
    // firstly we use the "fire and forget" strategy ,supposing it's will success

    // if success return true , or fail return false
}

