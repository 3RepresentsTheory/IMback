//
// Created by 28443 on 2023/8/24.
//

#ifndef DEMO02_FRIENDAPI_H
#define DEMO02_FRIENDAPI_H
#include "../service/FriendService.h"
#include "../service/UserService.h"
#include "../Utils/types.h"
#include "../Utils/utils.h"
#include <QtConcurrent/qtconcurrentrun.h>
#include "SessionApi.h"
#include "../service/GroupService.h"
#include "../service/MessageService.h"

#include <QtHttpServer/QHttpServer>

class FriendRequest{
public:
    FriendRequest(const string &username, const string &text);
    ~FriendRequest();
    static FriendRequest formJsonObject(const QJsonObject &json) ;
    string username;
    string text;
};

class AcceptRequest{
public:
    AcceptRequest(const string& id);
    ~AcceptRequest();
    static AcceptRequest fromJsonObject(const QJsonObject &json);
    string id;
};

class Requests{
public:
    Requests(const string& last);
    ~Requests();
    static Requests fromJsonObject(const QJsonObject &json);
    static QJsonArray toJsonObject(vector<map<string,string>>& rc);
    string last;
};

class FriendApi :public QObject{
    Q_OBJECT
private:
    FriendService* friendService;
    UserService *userService;
    //we use groupservice to add two people to one group
    GroupService *groupService;
    MessageService *messageService;
public:
    FriendApi(
            UserService* userService,
            FriendService* friendService1,
            GroupService*groupService,
            MessageService*messageService1);
    ~FriendApi();
    QHttpServerResponse request(const QHttpServerRequest &request);
    QHttpServerResponse accept(const QHttpServerRequest& request);
    QHttpServerResponse requests(const QHttpServerRequest &request);

signals:
    void passMessageToBroadCast(MsgLoad msg,QVector<qint64>glist);
};


#endif //DEMO02_FRIENDAPI_H
