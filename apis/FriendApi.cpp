//
// Created by 28443 on 2023/8/24.
//

#include "FriendApi.h"
#include <iostream>

FriendRequest::FriendRequest(
        const string &username,
        const string &text):username(username),text(text) {
}

FriendRequest::~FriendRequest() {

}

FriendRequest FriendRequest::formJsonObject(const QJsonObject &json) {
    map<string,string> reqParams = jsonToString(json);
    FriendRequest friendRequest = FriendRequest(reqParams["username"],reqParams["text"]);
    return friendRequest;
}


AcceptRequest::AcceptRequest(const string &id):id(id) {

}

AcceptRequest::~AcceptRequest() {

}

AcceptRequest AcceptRequest::fromJsonObject(const QJsonObject &json) {
    map<string,string> reqParams = jsonToString(json);
    AcceptRequest acceptRequest = AcceptRequest(reqParams["id"]);
    return acceptRequest;
}

Requests::Requests(const string &last) :last(last){

}

Requests::~Requests() {

}

Requests Requests::fromJsonObject(const QJsonObject &json) {
    map<string,string> reqParams = jsonToString(json);
    Requests requests = Requests(reqParams["last"]);
    return requests;
}

QJsonArray Requests::toJsonObject(vector<map<string, string>> &rc) {
    QJsonArray jsonArray;

    for (auto& request : rc) {
        QJsonObject requestObject;
        QString id = QString::fromStdString(request["id"]);
        QString time = QString::fromStdString(request["time"]);
        QString text = QString::fromStdString(request["text"]);
        QString uid = QString::fromStdString(request["uid"]);

        requestObject["id"] = id.toInt();
        requestObject["time"] = time.toInt();
        requestObject["text"] = text;
        requestObject["uid"] = uid.toInt();
        jsonArray.append(requestObject);
    }

    return jsonArray;
}

FriendApi::FriendApi(
        UserService* userService,
        FriendService *friendService,
        GroupService *groupService
) :
        userService(userService),
        friendService(friendService),
        groupService(groupService){}

FriendApi::~FriendApi() {
    if(!friendService){
        delete(friendService);
    }
}

QHttpServerResponse FriendApi::request(const QHttpServerRequest &request) {
    // auth
    QUuid token = QUuid::fromString(getcookieFromRequest(request).value().toStdString());
    auto  id    = SessionApi::getInstance()->getIdByCookie(token);
    if(!id.has_value())
        return QHttpServerResponse(QJsonObject{{"msg","身份验证失败。"}},QHttpServerResponder::StatusCode::Unauthorized);

    // json parse and field check
    const auto json = byteArrayToJsonObject(request.body());
    if (!json)
        return QHttpServerResponse(QJsonObject{{"msg","参数错误。"}}, QHttpServerResponder::StatusCode::BadRequest);

    FriendRequest friendRequest = FriendRequest::formJsonObject(json.value());
    string             username = friendRequest.username;
    string                 text = friendRequest.text;

    if(username.empty()){
        return QHttpServerResponse(QJsonObject{{"msg","username不能为空。"}},QHttpServerResponder::StatusCode::BadRequest);
    }

    // consistency check
    int targetId = userService->selectIdByName(username);
    if(targetId == -1){
        return QHttpServerResponse(QJsonObject{{"msg","请输入正确的用户名。"}},QHttpServerResponder::StatusCode::InternalServerError);
    }
    if(targetId==id.value()){
        return QHttpServerResponse(QJsonObject{{"msg","不能添加自己为好友。"}},QHttpServerResponder::StatusCode::InternalServerError);
    }
    bool exist = friendService->isFriendExists(to_string(id.value()),to_string(targetId));
    if(exist){
        return QHttpServerResponse(QJsonObject{{"msg","已拥有该好友。"}},QHttpServerResponder::StatusCode::InternalServerError);
    }

    // currently we simply accept it, no need to wait for the target user to accept
    // add metadata to twin group
    Group twingroup;
    int groupid;
    int minid = min(id.value(),targetId);int maxid = max(id.value(),targetId);
    twingroup.name = QString::fromStdString(to_string(minid)+','+to_string(maxid));  //????
    twingroup.owner= 0;   //????
    twingroup.type = "twin";

    //create group and join (TODO:need tx)
    if(
            !groupService->CreateGroup(twingroup,groupid)||
            !groupService->JoinGroup(minid,groupid)||
            !groupService->JoinGroup(maxid,groupid)
    ){
        return QHttpServerResponse(QJsonObject{{"msg","创建群组失败"}},QHttpServerResponder::StatusCode::InternalServerError);
    }

//    bool rc = friendService->insertRequest(to_string(targetId),to_string(id.value()),text);
//    if (!rc)
//        return QHttpServerResponse(QJsonObject{{"msg","服务器内部错误。"}}, QHttpServerResponder::StatusCode::InternalServerError);
    return QHttpServerResponse("");
}


// currently abandoned
QHttpServerResponse FriendApi::accept(const QHttpServerRequest &request) {
    QUuid token = QUuid::fromString(getcookieFromRequest(request).value().toStdString());
    auto id = SessionApi::getInstance()->getIdByCookie(token);
    if(!id.has_value())
        return QHttpServerResponse(QJsonObject{{"msg","身份验证失败。"}},QHttpServerResponder::StatusCode::Unauthorized);
    const auto json = byteArrayToJsonObject(request.body());
    if (!json)
        return QHttpServerResponse(QJsonObject{{"msg","参数错误。"}}, QHttpServerResponder::StatusCode::BadRequest);
    AcceptRequest acceptRequest = AcceptRequest::fromJsonObject(json.value());
    string requestId = acceptRequest.id;
    map<string ,string > rc = friendService->seletInfoByRequestId(requestId);
    if(rc.empty()||stoi(rc["userId"])!=id){
        return QHttpServerResponse(QJsonObject{{"msg","没有该好友请求或请求已经被接受。"}},QHttpServerResponder::StatusCode::InternalServerError);
    }


    bool updaterc = friendService->acceptRequest(to_string(id.value()),rc["requestUserId"],requestId);
    if(!updaterc)
        return QHttpServerResponse(QJsonObject{{"msg","服务器内部错误。"}},QHttpServerResponder::StatusCode::InternalServerError);
    return QHttpServerResponse("");
}

// currently abandoned
QHttpServerResponse FriendApi::requests(const QHttpServerRequest &request) {
    QUuid token = QUuid::fromString(getcookieFromRequest(request).value().toStdString());
    auto id = SessionApi::getInstance()->getIdByCookie(token);
    if(!id.has_value())
        return QHttpServerResponse(QJsonObject{{"msg","身份验证失败。"}},QHttpServerResponder::StatusCode::Unauthorized);
    string last = request.query().queryItemValue("last").toStdString();
    vector<map<string ,string >> rc = friendService->getRequests(to_string(id.value()),last);
    QJsonArray qJsonArray = Requests::toJsonObject(rc);
    return QHttpServerResponse(qJsonArray);
}


