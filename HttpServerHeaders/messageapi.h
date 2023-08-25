//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_MESSAGEAPI_H
#define DEMO02_MESSAGEAPI_H


#include <QHttpServerRequest>
#include <QHttpServerResponse>

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
    // 32 bit for compatibility in sqlite
    qint32  id;          //ret string in api
    QString type;        //from request
    QString content;     //from request
    qint32  time;
    qint32  uid;
    qint32  mid;         //string in api
    qint32  gid;         //ret from request

    bool fromQJsonObject(const QJsonObject &) override;
    QJsonObject toQJsonObject() override;
};




class MessageApi {
public:
    MessageApi(){};
    QHttpServerResponse handleSentMessageRequest(const QHttpServerRequest &request);
private:
    int broadcastMessageToGroup(int gid, QString messagetext);
};


#endif //DEMO02_MESSAGEAPI_H
