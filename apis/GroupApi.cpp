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
        return QHttpServerResponse("创建群组表单错误", QHttpServerResponder::StatusCode::BadRequest);


    // get the cookie and auth it, fail then return
    auto cookie =  getcookieFromRequest(request);
    auto uid = (cookie.has_value())?
               SessionApi::getInstance()->getIdByCookie(QUuid::fromString(cookie.value())):
               (std::nullopt);

    if(!uid.has_value())
        return QHttpServerResponse("身份验证失败", QHttpServerResponder::StatusCode::BadRequest);

    // json field validating make sure gid, type ,content is not blank
    if(!group.fromQJsonObject(json.value())){
        return QHttpServerResponse("创建群组表单错误或为空", QHttpServerResponder::StatusCode::BadRequest);
    }
    group.owner = uid.value();
    // default group is public
    group.type  = "public";

    int gid;
    if(!groupService->CreateGroup(group,gid)){
        return QHttpServerResponse("服务器内部错误，请稍后再试", QHttpServerResponder::StatusCode::InternalServerError);
    }
    group.id = gid;

    // join myself to the group (need tx)
    if(!groupService->JoinGroup(uid.value(),gid))
        return QHttpServerResponse("服务器内部错误，无法加入群组，稍后再试", QHttpServerResponder::StatusCode::BadRequest);

    return QHttpServerResponse(group.toQJsonObject());
}

QHttpServerResponse GroupApi::joinGroup(const QHttpServerRequest &request) {
    Group group;
    // auth the request
    const auto json = byteArrayToJsonObject(request.body());
    if (!json)
        return QHttpServerResponse("接收消息失败或为空", QHttpServerResponder::StatusCode::BadRequest);

    if(!json.value().contains("gid")||json.value().value("gid").isNull())
        return QHttpServerResponse("接收消息失败或为空", QHttpServerResponder::StatusCode::BadRequest);
    int gid = json.value().value("gid").toInt();

    // get the cookie and auth it, fail then return
    auto cookie =  getcookieFromRequest(request);
    auto uid = (cookie.has_value())?
               SessionApi::getInstance()->getIdByCookie(QUuid::fromString(cookie.value())):
               (std::nullopt);
    if(!uid.has_value())
        return QHttpServerResponse("身份验证失败", QHttpServerResponder::StatusCode::BadRequest);

    //need check whether it join for twice
    if(!groupService->JoinGroup(gid,uid.value()))
        return QHttpServerResponse("服务器内部错误，无法加入群组，稍后再试", QHttpServerResponder::StatusCode::BadRequest);

    return QHttpServerResponse(groupService->GetGroupHasjoin(gid));
}

QHttpServerResponse GroupApi::getGroupHasjoin(const QHttpServerRequest &request) {
    auto query = request.query();
    bool is_num;
    qint64 ddl = query.queryItemValue("ddl").toInt(&is_num);
    if(query.isEmpty()|| !is_num){
        return QHttpServerResponse("请检查请求参数",QHttpServerResponder::StatusCode::BadRequest);
    }

    // get the cookie and auth it, fail then return
    auto cookie =  getcookieFromRequest(request);
    auto uid = (cookie.has_value())?
               SessionApi::getInstance()->getIdByCookie(QUuid::fromString(cookie.value())):
               (std::nullopt);

    if(!uid.has_value())
        return QHttpServerResponse("身份验证失败", QHttpServerResponder::StatusCode::BadRequest);

    return QHttpServerResponse(groupService->GetGroupHasjoin(uid.value(),ddl));
}

QHttpServerResponse GroupApi::getGroupUserList(const QHttpServerRequest &request) {
    auto query = request.query();
    bool is_num;
    qint64 gid = query.queryItemValue("gid").toInt(&is_num);
    if(query.isEmpty()|| !is_num){
        return QHttpServerResponse("请检查请求参数",QHttpServerResponder::StatusCode::BadRequest);
    }
    QJsonArray jsonArray;
    for(auto id: msgService->GetGroupUserList(gid)){
        jsonArray.append(id);
    }
    return QHttpServerResponse(jsonArray);
}
