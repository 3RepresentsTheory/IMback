//
// Created by no3core on 2023/8/24.
//

#include "GroupApi.h"
#include "SessionApi.h"

QHttpServerResponse GroupApi::createGroup(const QHttpServerRequest &request) {
    Group group;
    // auth the request
    const auto json = byteArrayToJsonObject(request.body());
    if (!json)
        return QHttpServerResponse(QJsonObject{{"msg","创建群组表单错误"}}, QHttpServerResponder::StatusCode::BadRequest);


    // get the cookie and auth it, fail then return
    auto cookie =  getcookieFromRequest(request);
    auto uid = (cookie.has_value())?
               SessionApi::getInstance()->getIdByCookie(QUuid::fromString(cookie.value())):
               (std::nullopt);

    if(!uid.has_value())
        return QHttpServerResponse(QJsonObject{{"msg","身份验证失败"}}, QHttpServerResponder::StatusCode::BadRequest);

    // json field validating make sure gid, type ,content is not blank
    if(!group.fromQJsonObject(json.value())){
        return QHttpServerResponse(QJsonObject{{"msg","创建群组表单错误或为空"}}, QHttpServerResponder::StatusCode::BadRequest);
    }
    group.owner = uid.value();
    // default group is public
    group.type  = "public";

    int gid;
    if(!groupService->CreateGroup(group,gid)){
        return QHttpServerResponse(QJsonObject{{"msg","服务器内部错误，请稍后再试"}}, QHttpServerResponder::StatusCode::InternalServerError);
    }
    group.id = gid;

    // join myself to the group (need tx)
    //need check whether it join for twice
    if(groupService->IsJoined(uid.value(),gid))
        return QHttpServerResponse(QJsonObject{{"msg","您已加入该群组"}}, QHttpServerResponder::StatusCode::BadRequest);
    if(!groupService->JoinGroup(uid.value(),gid))
        return QHttpServerResponse(QJsonObject{{"msg","服务器内部错误，无法加入群组，稍后再试"}}, QHttpServerResponder::StatusCode::BadRequest);

    // send a welcome msg to myself:

    // basic fields of message
    Message message;
    message.type    = "text";
    message.content ="群"+group.name+"创建完毕";
    message.gid     = gid;

    int message_id = 0;
    message.uid = uid.value();
    if(!msgService->StoreMessage(message, message_id)){
        return QHttpServerResponse(QJsonObject{{"msg","服务器内部错误，请稍后再试"}}, QHttpServerResponder::StatusCode::InternalServerError);
    }

    message.id  = message_id;
    msgService->FillMessageFromDB(message);
    // add this message as last message to the group
    group.last_message = message;

    emit passMessageToBroadCast(MsgLoad(new Message(message)), {uid.value()});

    return QHttpServerResponse(group.toQJsonObjectWithLastMsg());
}

QHttpServerResponse GroupApi::joinGroup(const QHttpServerRequest &request) {
    Group group;
    // auth the request
    const auto json = byteArrayToJsonObject(request.body());
    if (!json)
        return QHttpServerResponse(QJsonObject{{"msg","接收消息失败或为空"}}, QHttpServerResponder::StatusCode::BadRequest);

    if(!json.value().contains("gid")||json.value().value("gid").isNull())
        return QHttpServerResponse(QJsonObject{{"msg","接收消息失败或为空"}}, QHttpServerResponder::StatusCode::BadRequest);
    int gid = json.value().value("gid").toInt();

    // get the cookie and auth it, fail then return
    auto cookie =  getcookieFromRequest(request);
    auto uid = (cookie.has_value())?
               SessionApi::getInstance()->getIdByCookie(QUuid::fromString(cookie.value())):
               (std::nullopt);
    if(!uid.has_value())
        return QHttpServerResponse(QJsonObject{{"msg","身份验证失败"}}, QHttpServerResponder::StatusCode::BadRequest);


    //need to check whether the target group is a twin group
    if(groupService->IsTwinGroup(gid)||groupService->IsGroupExist(gid)){
        return QHttpServerResponse(QJsonObject{{"msg","不存在该群聊"}}, QHttpServerResponder::StatusCode::BadRequest);
    }

    //need check whether it join for twice
    if(groupService->IsJoined(uid.value(),gid))
        return QHttpServerResponse(QJsonObject{{"msg","您已加入该群组"}}, QHttpServerResponder::StatusCode::BadRequest);

    if(!groupService->JoinGroup(uid.value(),gid))
        return QHttpServerResponse(QJsonObject{{"msg","服务器内部错误，无法加入群组，稍后再试"}}, QHttpServerResponder::StatusCode::BadRequest);


    return QHttpServerResponse(groupService->GetGroupHasjoin(gid));
}

QHttpServerResponse GroupApi::getGroupHasjoin(const QHttpServerRequest &request) {
    auto query = request.query();
    bool is_num;
    qint64 ddl = query.queryItemValue("ddl").toInt(&is_num);
    if(query.isEmpty())
        ddl = 0;
    else if(!is_num)
        return QHttpServerResponse(QJsonObject{{"msg","请检查请求参数"}},QHttpServerResponder::StatusCode::BadRequest);

    // get the cookie and auth it, fail then return
    auto cookie =  getcookieFromRequest(request);
    auto uid = (cookie.has_value())?
               SessionApi::getInstance()->getIdByCookie(QUuid::fromString(cookie.value())):
               (std::nullopt);

    if(!uid.has_value())
        return QHttpServerResponse(QJsonObject{{"msg","身份验证失败"}}, QHttpServerResponder::StatusCode::BadRequest);

    return QHttpServerResponse(groupService->GetGroupHasjoin(uid.value(),ddl));
}

QHttpServerResponse GroupApi::getGroupUserList(const QHttpServerRequest &request) {
    auto query = request.query();
    bool is_num;
    qint64 gid = query.queryItemValue("gid").toInt(&is_num);
    if(query.isEmpty()|| !is_num){
        return QHttpServerResponse(QJsonObject{{"msg","请检查请求参数"}},QHttpServerResponder::StatusCode::BadRequest);
    }
    QJsonArray jsonArray;
    for(auto id: msgService->GetGroupUserList(gid)){
        jsonArray.append(id);
    }
    return QHttpServerResponse(jsonArray);
}
