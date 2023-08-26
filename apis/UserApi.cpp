//
// Created by no3core on 2023/8/24.
//

#include "UserApi.h"
#include "../Utils/utils.h"


User::User(const string &username, const string &password, const string &nickname, const string &color,
           const string &avatar) : username(username), password(password),
           nickname(nickname), color(color),avatar(avatar) {}

User::~User() {

}

UserApi::UserApi(UserService *userService) :
    userService(userService) {}

User User::formJsonObject(const QJsonObject &json){
    map<string,string> reqParams = jsonToString(json);
    User user = User(
            reqParams["username"],
            reqParams["password"],
            reqParams["nickname"],
            reqParams["color"],
            reqParams["avatar"]
    );
    return user;
}

QJsonObject User::toJsonObject(const string &nickname,const QUuid& token) {
    return QJsonObject{
            {"nickname",QString::fromStdString(nickname)},
            {"token",token.toString(QUuid::StringFormat::WithoutBraces)}
    };
}



QHttpServerResponse UserApi:: registerSession(const QHttpServerRequest &request) {
    const auto json = byteArrayToJsonObject(request.body());

    if (!json)
        return QHttpServerResponse("Error", QHttpServerResponder::StatusCode::BadRequest);
    User user = User::formJsonObject(json.value());
    string username = user.username;
    string password = user.password;
    string nickname = user.nickname;
    if (username.empty()||
        password.empty()||
        nickname.empty())
        return QHttpServerResponse("请输入完整的username,password,nickname,且不能为空",QHttpServerResponder::StatusCode::BadRequest);

    if (userService->isUsernameExists(username)) {
        return QHttpServerResponse("用户名已存在，请重试",QHttpServerResponder::StatusCode::BadRequest);
    }

    if (userService->isNicknameExists(nickname)) {
        return QHttpServerResponse("昵称已存在，请重试", QHttpServerResponder::StatusCode::BadRequest);
    }
    userService->insertUser(username,password,nickname);

    return QHttpServerResponse("Success");

}

QHttpServerResponse UserApi:: login(const QHttpServerRequest &request) {
    const auto json = byteArrayToJsonObject(request.body());

    if (!json)
        return QHttpServerResponse("Error", QHttpServerResponder::StatusCode::BadRequest);
    User user = User::formJsonObject(json.value());
    string username = user.username;
    string password = user.password;
    if (username.empty()||
        password.empty()){
        return QHttpServerResponse(
                "请输入完整的username,password,且不能为空",
                QHttpServerResponder::StatusCode::BadRequest
        );
    }

    map<string ,string > rc =
            userService->validateUserCredentials(username,password);
    if(rc.empty())
        return QHttpServerResponse("用户名或密码错误",QHttpServerResponder::StatusCode::BadRequest);

    SessionEntry sessionEntry = SessionApi::getInstance()->createEntryAndStart(stoi(rc["id"]));

    return QHttpServerResponse(User::toJsonObject(rc["nickname"],sessionEntry.token.value()));
}

QHttpServerResponse UserApi::info(const QHttpServerRequest &request) {
    QUuid token = QUuid::fromString(getcookieFromRequest(request).value().toStdString());
    auto id = SessionApi::getInstance()->getIdByCookie(token);
    if(!id.has_value())
        return QHttpServerResponse("身份验证失败",QHttpServerResponder::StatusCode::BadRequest);
    const auto json = byteArrayToJsonObject(request.body());
    if (!json)
        return QHttpServerResponse("Error", QHttpServerResponder::StatusCode::BadRequest);

    User user = User::formJsonObject(json.value());
    string nickname = user.nickname;
    string color = user.color;
    string avatar = user.avatar;
    bool rc = userService->updateInfo(id.value(),nickname,color,avatar);
    if(!rc){
        return QHttpServerResponse("Error", QHttpServerResponder::StatusCode::BadRequest);
    }
    return QHttpServerResponse("OK");
}



//
//QHttpServerResponse UserApi::logout(const QHttpServerRequest &request) {
//    const auto maybecookie = getcookieFromRequest(request);
//    if (!maybecookie)
//        return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
//
//    auto maybeSession = std::find(sessions.begin(), sessions.end(), *maybecookie);
//    if (maybeSession != sessions.end())
//        maybeSession->endSession();
//    return QHttpServerResponse(QHttpServerResponder::StatusCode::Ok);
//}
