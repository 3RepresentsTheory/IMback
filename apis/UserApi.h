//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_USERAPI_H
#define DEMO02_USERAPI_H
#include "../service/UserService.h"
#include "../Utils/Jsonable.h"
#include "SessionApi.h"

class User{
public:
    string username;
    string password;
    string nickname;
    string color;
    string avatar;

    User(const string &username, const string &password, const string &nickname,
         const string &color,const string &avatar);
    ~User();
    static User formJsonObject(const QJsonObject &json) ;
    static QJsonObject toJsonObject(const string& nickname,const QUuid& token) ;
};

class UserApi {
private:
    UserService* userService;
    SessionApi* sessionApi;
public:

    UserApi(UserService *userService, SessionApi *sessionApi);

    QHttpServerResponse registerSession(const QHttpServerRequest &request);
    QHttpServerResponse login(const QHttpServerRequest &request);
//    QHttpServerResponse logout(const QHttpServerRequest &request);

    QHttpServerResponse info(const QHttpServerRequest &request);
};


#endif //DEMO02_USERAPI_H
