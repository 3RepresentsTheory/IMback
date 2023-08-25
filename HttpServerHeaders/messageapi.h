//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_MESSAGEAPI_H
#define DEMO02_MESSAGEAPI_H


#include "../service/messageService.h"
#include "utils.h"
#include "../Dao/BaseDao.h"

class Jsonable{
public:
    // return a new qjson object
    virtual QJsonObject toQJsonObject() = 0;

    // fill fields by a qjson object
    virtual bool fromQJsonObject(const QJsonObject& ) = 0;
};

class Message:Jsonable{
public:
    Message(){};

    Message(map<string,string> row){
        id      = std::stoi(row["id"]);
        type    = QString::fromStdString(row["type"]);
        content = QString::fromStdString(row["content"]);
        time    = getUnixTimeStampFromString(row["time"]);
        uid     = std::stoi(row["uid"]);
        mid     = std::stoi(row["mid"]);
        gid     = std::stoi(row["gid"]);
    };

    qint64  id;          //ret string in api
    QString type;        //from request
    QString content;     //from request
    qint64  time;
    qint64  uid;
    qint64  mid;         //string in api
    qint64  gid;         //ret from request

    bool fromQJsonObject(const QJsonObject &) override;
    QJsonObject toQJsonObject() override;

};




class MessageApi {
public:

    MessageApi(){};
    QHttpServerResponse handleSentMessageRequest(const QHttpServerRequest &request);
private:
    bool broadcastMessageToGroup(Message message);
    MessageService service;
};


#endif //DEMO02_MESSAGEAPI_H
