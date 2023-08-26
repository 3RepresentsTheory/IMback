//
// Created by no3core on 2023/8/24.
//

#include "MessageApi.h"
#include "../Utils/utils.h"


QHttpServerResponse MessageApi::handleSentMessageRequest(const QHttpServerRequest &request) {
    Message message;
    // json convert validating
    const auto json = byteArrayToJsonObject(request.body());

    if (!json)
        return QHttpServerResponse("接收消息失败或为空", QHttpServerResponder::StatusCode::BadRequest);

    auto cookie =  getcookieFromRequest(request);
    auto uid = (cookie.has_value())?
            SessionApi::getInstance()->getIdByCookie(QUuid::fromString(cookie.value())):
            (std::nullopt);
    // get the cookie and auth it, fail then return
    if(!uid.has_value())
        return QHttpServerResponse("Authentication failed", QHttpServerResponder::StatusCode::BadRequest);

    // json field validating make sure gid, type ,content is not blank
    if(!message.fromQJsonObject(json.value())){
        return QHttpServerResponse("接收消息失败或为空", QHttpServerResponder::StatusCode::BadRequest);
    }

    // special validate for json field: is content blank? is gid valid? is uid is in the group gid?
    // TODO: add validate for content gid uid content

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
    int message_id = 0;
    if(!messageService->StoreMessage(message, message_id)){
        return QHttpServerResponse("Internal server error", QHttpServerResponder::StatusCode::InternalServerError);
    }

    message.id  = message_id;
    message.uid = uid.value();
    messageService->FillMessageFromDB(message);

    // check broadcast if success return jsonfy object, or fail simply return 400
    if(broadcastMessageToGroup(message)){
        // I dont know how to handle here ...
    }

    return QHttpServerResponse(message.toQJsonObject());

}

bool MessageApi::broadcastMessageToGroup(Message message) {
    // access database to generate a broadcast list (gid -> [uid1,uid2...])
    // here we ignore if someone add to this group and he miss it
    // no cache for this, simply read from database
    qint32 gid = message.gid;

    // call the chatserver to broadcast using some method (we assume this method is
    // atomically -- its will all send/or not send at all)
    // firstly we use the "fire and forget" strategy ,supposing it's will success

    // broadcastserver*broadcastByWS(message,service.GetGroupUserList(gid))

    // if success return true , or fail return false
}

QHttpServerResponse MessageApi::retrieveHistoryMsgList(const QHttpServerRequest &request) {
    HistoryRqst historyrqst;
    const auto json = byteArrayToJsonObject(request.body());

    if (!json)
        return QHttpServerResponse("接收消息失败或为空", QHttpServerResponder::StatusCode::BadRequest);

    auto cookie =  getcookieFromRequest(request);
    auto uid = (cookie.has_value())?
               SessionApi::getInstance()->getIdByCookie(QUuid::fromString(cookie.value())):
               (std::nullopt);
    // get the cookie and auth it, fail then return
    if(!uid.has_value())
        return QHttpServerResponse("Authentication failed", QHttpServerResponder::StatusCode::BadRequest);

    // json field validating make sure gid, type ,content is not blank
    if(!historyrqst.fromQJsonObject(json.value())){
        return QHttpServerResponse("接收消息失败或为空", QHttpServerResponder::StatusCode::BadRequest);
    }

    // special validate for json field: is content blank? is gid valid? is uid is in the group gid?
    // TODO: add validate for content gid uid content

    return QHttpServerResponse(
            messageService->GetMessagelistByTime(
                historyrqst.mid,
                historyrqst.gid
            )
    );
}
