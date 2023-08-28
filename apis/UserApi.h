//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_USERAPI_H
#define DEMO02_USERAPI_H
#include "../service/UserService.h"
#include "SessionApi.h"
#include <QHostAddress>

class User{
public:
    int id;
    string username;
    string password;
    string nickname;
    string color;
    string avatar;
    User();
    User(map<string,string>& userInfo);
    ~User();
    static User formJsonObject(const QJsonObject &json) ;
    static QJsonObject toJsonObject(const User& user);
    static QJsonObject toJsonObjectForLogin(const User& user,const QUuid& token);
    static QJsonArray toJsonObjectForInfos(const vector<User>& rc);
    static QJsonObject toJsonObjectForUserip(const string& ipPortString);
};

class UserService;
class UserApi {
private:
    UserService* userService;
public:

    UserApi(UserService *userService);

    QHttpServerResponse registerSession(const QHttpServerRequest &request);
    QHttpServerResponse login(const QHttpServerRequest &request);
//    QHttpServerResponse logout(const QHttpServerRequest &request);

    QHttpServerResponse info(const QHttpServerRequest &request);
    QHttpServerResponse infos(const QHttpServerRequest &request);
    QHttpServerResponse getUserip(const QHttpServerRequest &request);
    QHttpServerResponse onlines(const QHttpServerRequest &request);
};


#endif //DEMO02_USERAPI_H
