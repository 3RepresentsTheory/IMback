//
// Created by no3core on 2023/8/24.
//

#include "MessageApi.h"
#include "../utils/utils.h"


QHttpServerResponse MessageApi::handleSentMessageRequest(const QHttpServerRequest &request) {
    Message message;
    // json convert validating
    const auto json = byteArrayToJsonObject(request.body());

    if (!json)
        return QHttpServerResponse(QJsonObject{{"msg","接收消息失败或为空"}}, QHttpServerResponder::StatusCode::BadRequest);

    // get the cookie and auth it, fail then return
    auto cookie =  getcookieFromRequest(request);
    auto uid = (cookie.has_value())?
            SessionApi::getInstance()->getIdByCookie(QUuid::fromString(cookie.value())):
            (std::nullopt);
    if(!uid.has_value())
        return QHttpServerResponse(QJsonObject{{"msg","身份验证失败"}}, QHttpServerResponder::StatusCode::BadRequest);

    // json field validating make sure gid, type ,content is not blank
    if(!message.fromQJsonObject(json.value())){
        return QHttpServerResponse(QJsonObject{{"msg","接收消息失败或为空"}}, QHttpServerResponder::StatusCode::BadRequest);
    }

    // special validate for json field: is content blank? is gid valid? is uid is in the group gid?
    // TODO: add validate for content gid uid content
    auto groupUserList = messageService->GetGroupUserList(message.gid);
    if(groupUserList.isEmpty()||!groupUserList.contains(uid.value()))
        return QHttpServerResponse(QJsonObject{{"msg","您不在该群组内或该群不存在"}}, QHttpServerResponder::StatusCode::BadRequest);

    // generate complete database object(store to database ,and have a copy in memory)
    // including:
    /*
     * type         (fill in)
     * content      (fill in)
     * gid          (fill in)
     * uid          (generate - map cookie to id, use sessionapi needed)
     * id           (generate - database)
     * time         (generate - database)
     * mid          (generate - database)
     */
    // we simply fill type content gid uid(use session api) ,store to database and read it
    // no transaction mechanism proctect
    int message_id = 0;
    message.uid = uid.value();
    if(!messageService->StoreMessage(message, message_id)){
        return QHttpServerResponse(QJsonObject{{"msg","服务器内部错误，请稍后再试"}}, QHttpServerResponder::StatusCode::InternalServerError);
    }

    message.id  = message_id;
    messageService->FillMessageFromDB(message);

    // check broadcast if success return jsonfy object, or fail simply return 400
    // use copy to avoid the message object is free when passing signal
    emit passMessageToBroadCast(message, groupUserList);

    return QHttpServerResponse(message.toQJsonObject());

}

//bool MessageApi::broadcastMessageToGroup(Message message) {
//    // access database to generate a broadcast list (gid -> [uid1,uid2...])
//    // here we ignore if someone add to this group and he miss it
//    // no cache for this, simply read from database
//    qint32 gid = message.gid;
//
//    // call the chatserver to broadcast using some method (we assume this method is
//    // atomically -- its will all send/or not send at all)
//    // firstly we use the "fire and forget" strategy ,supposing it's will success
//
//
//    // if success return true , or fail return false
//}

QHttpServerResponse MessageApi::retrieveHistoryMsgList(const QHttpServerRequest &request) {

    auto query = request.query();
    bool is_num0;bool is_num1;bool is_num2;
    qint64 gid   = query.queryItemValue("gid").toInt(&is_num0);
    qint64 start = query.queryItemValue("start").toInt(&is_num1);
    qint64 end   = query.queryItemValue("end").toInt(&is_num2);
    if(query.isEmpty()||
        !(is_num0)||
        !(is_num1||is_num2)|| // make sure there must be a start or end
        (is_num1&&is_num2)&&start > end //only check when they are both number
    ){
        return QHttpServerResponse(QJsonObject{{"msg","请检查请求参数"}},QHttpServerResponder::StatusCode::BadRequest);
    }

    // get the cookie and auth it, fail then return
    auto cookie =  getcookieFromRequest(request);
    auto uid = (cookie.has_value())?
               SessionApi::getInstance()->getIdByCookie(QUuid::fromString(cookie.value())):
               (std::nullopt);
    if(!uid.has_value())
        return QHttpServerResponse(QJsonObject{{"msg","身份验证失败"}}, QHttpServerResponder::StatusCode::BadRequest);

    // check whether exist a group or user is in this group
    auto groupUserList = messageService->GetGroupUserList(gid);
    if(groupUserList.isEmpty()||!groupUserList.contains(uid.value()))
        return QHttpServerResponse(QJsonObject{{"msg","您不在该群组内或该群不存在"}}, QHttpServerResponder::StatusCode::BadRequest);

    if(!is_num2) end = start + MAX_HISTORY_RETRIEVE;
    if(!is_num1) start = ((end - MAX_HISTORY_RETRIEVE)<0)?0:(end - MAX_HISTORY_RETRIEVE);

    return QHttpServerResponse(
            messageService->GetMessagelistByPeriod(gid, start, end)
    );
}
