//
// Created by 28443 on 2023/8/24.
//

#include "FriendApi.h"
#include <iostream>

FriendRequest::FriendRequest(const string &username, const string &text):username(username),text(text) {

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

QJsonArray Requests::toJsonObject(const vector<map<string, string>> &rc) {
    QJsonArray jsonArray;

    for (const auto& request : rc) {
        QJsonObject requestObject;
        QString id = QString::fromStdString(request.at("id"));
        QString time = QString::fromStdString(request.at("time"));
        QString text = QString::fromStdString(request.at("text"));
        QString uid = QString::fromStdString(request.at("uid"));

        requestObject["id"] = id.toInt();
        requestObject["time"] = time.toInt();
        requestObject["text"] = text;
        requestObject["uid"] = uid.toInt();
        jsonArray.append(requestObject);
    }

    return jsonArray;
}

FriendApi::FriendApi(UserService* userService,FriendService *friendService, SessionApi *sessionApi) : userService(userService),friendService(friendService), sessionApi(sessionApi) {}

FriendApi::~FriendApi() {
    if(!friendService){
        delete(friendService);
    }
}

QHttpServerResponse FriendApi::request(const QHttpServerRequest &request) {
    QUuid token = QUuid::fromString(getcookieFromRequest(request).value().toStdString());
    int id = sessionApi->getIdByCookie(token);
    if(id==-1)
        return QHttpServerResponse("身份验证失败",QHttpServerResponder::StatusCode::BadRequest);
    const auto json = byteArrayToJsonObject(request.body());
    if (!json)
        return QHttpServerResponse("Error", QHttpServerResponder::StatusCode::BadRequest);
    FriendRequest friendRequest = FriendRequest::formJsonObject(json.value());
    string username = friendRequest.username;
    string text = friendRequest.text;
    if(username.empty()){
        return QHttpServerResponse("username不能为空",QHttpServerResponder::StatusCode::BadRequest);
    }
    int targetId = userService->selectIdByName(username);
    if(targetId == -1){
        return QHttpServerResponse("请输入正确的用户名",QHttpServerResponder::StatusCode::BadRequest);
    }
    if(targetId==id){
        return QHttpServerResponse("不能添加自己为好友",QHttpServerResponder::StatusCode::BadRequest);
    }
    bool rc = friendService->insertRequest(to_string(targetId),to_string(id),text);
    if (!rc)
        return QHttpServerResponse("Error", QHttpServerResponder::StatusCode::BadRequest);
    return QHttpServerResponse("OK");
}

QHttpServerResponse FriendApi::accept(const QHttpServerRequest &request) {
    QUuid token = QUuid::fromString(getcookieFromRequest(request).value().toStdString());
    int id = sessionApi->getIdByCookie(token);
    if(id==-1)
        return QHttpServerResponse("身份验证失败",QHttpServerResponder::StatusCode::BadRequest);
    const auto json = byteArrayToJsonObject(request.body());
    if (!json)
        return QHttpServerResponse("Error", QHttpServerResponder::StatusCode::BadRequest);
    AcceptRequest acceptRequest = AcceptRequest::fromJsonObject(json.value());
    string requestId = acceptRequest.id;
    map<string ,string > rc = friendService->seletInfoByRequestId(requestId);
    if(rc.empty()||stoi(rc["userId"])!=id){
        return QHttpServerResponse("没有该好友请求或请求已经被接受",QHttpServerResponder::StatusCode::BadRequest);
    }
    bool updaterc = friendService->acceptRequest(to_string(id),rc["requestUserId"],requestId);
    if(!updaterc)
        return QHttpServerResponse("Error",QHttpServerResponder::StatusCode::BadRequest);
    return QHttpServerResponse("OK");
}

QHttpServerResponse FriendApi::requests(const QHttpServerRequest &request) {
    QUuid token = QUuid::fromString(getcookieFromRequest(request).value().toStdString());
    int id = sessionApi->getIdByCookie(token);
    if(id==-1)
        return QHttpServerResponse("身份验证失败",QHttpServerResponder::StatusCode::BadRequest);
    string last = request.query().queryItemValue("last").toStdString();
    vector<map<string ,string >> rc = friendService->getRequests(to_string(id),last);
    QJsonArray qJsonArray = Requests::toJsonObject(rc);
    return QHttpServerResponse(qJsonArray);
}


