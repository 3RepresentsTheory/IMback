//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_USERAPI_H
#define DEMO02_USERAPI_H
#include "../service/userService.h"
#include "jsonable.h"
#include "sessionapi.h"

class User{
public:
    string username;
    string password;
    string nickname;
    User(const string &username, const string &password, const string &nickname);

    static User formJsonObject(const QJsonObject &json) ;
    static QJsonObject toJsonObject(const string& nickname,const QUuid& token) ;

    virtual ~User();
};

class UserApi {
private:
    UserService* userService;
public:
    QHttpServerResponse registerSession(const QHttpServerRequest &request);
    QHttpServerResponse login(const QHttpServerRequest &request);
//    QHttpServerResponse logout(const QHttpServerRequest &request);

};


#endif //DEMO02_USERAPI_H
