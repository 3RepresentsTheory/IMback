//
// Created by no3core on 2023/8/25.
//

#ifndef DEMO02_DATACLASSES_H
#define DEMO02_DATACLASSES_H

#include <QJsonObject>
#include "../Utils/utils.h"


class Jsonable{
public:
    // return a new qjson object
    virtual QJsonObject toQJsonObject() =0;

    // fill fields by a qjson object
    virtual bool fromQJsonObject(const QJsonObject& ) = 0;

    // get the object type
    virtual QString getType() = 0 ;

    virtual ~Jsonable() {}
};

// a containter to pass abstract class,
// usage:
// emit passMessageToBroadCast(MsgLoad(new Jsonable(target),target_list))
class MsgLoad {
public:
    MsgLoad( Jsonable*loadcontent):
        loadcontent(loadcontent){};
    ~MsgLoad() {}
    // sorry for this... I'm pretty much confused
    // with c++ lifetime management and unique_ptr/move semantic,
    // so I just use this to avoid memory leak
    void FreeContent(){delete loadcontent;}
    Jsonable*  loadcontent;
};

using dao_entry = map<string,string> ;

// message service - api
class Message:public Jsonable{
public:
    Message(){};

    Message(dao_entry row,bool isFromGroupCstr=0);

    qint64  id;          //ret string in api
    QString type;        //from request
    QString content;     //from request
    qint64  time;
    qint64  uid;
    qint64  mid;         //string in api
    qint64  gid;         //ret from request

    bool fromQJsonObject(const QJsonObject &) override;
    QJsonObject toQJsonObject() override;
    QString getType() override {return "message";};

    friend std::ostream& operator<<(std::ostream& os, const  Message &rhs) {
        os << "Message: " << &rhs << std::endl;
        os << "id: " <<     rhs.id << std::endl;
        os << "type: " <<   rhs.type.toStdString() << std::endl;
        os << "content: " <<rhs.content.toStdString() << std::endl;
        os << "time: " <<   rhs.time << std::endl;
        os << "uid: " <<    rhs.uid << std::endl;
        os << "mid: " <<    rhs.mid << std::endl;
        os << "gid: " <<    rhs.gid << std::endl;

        return os;
    }

};

class HistoryRqst:public Jsonable{
public:
    HistoryRqst(){};
    bool fromQJsonObject(const QJsonObject &) override;
    QJsonObject toQJsonObject() override;
    QString getType() override {return "history request";};

    qint64  mid;         //string in api
    qint64  gid;         //ret from request
};


class Group:public Jsonable{
public:
    qint64 id;          //gcid in database
    QString name;
    qint64 owner;
    QString type;       //gctype in database
    QString avatar;
    QString color;
    Message last_message; // can be null

    Group(){};
    Group(dao_entry row,bool isContainLastMessage);

    QString getType() override {return "group";};
    bool fromQJsonObject(const QJsonObject &) override;
    QJsonObject toQJsonObject() override;
    QJsonObject toQJsonObjectWithLastMsg();

};


class WsAuth:public Jsonable{
public:
    QString cookie;
    QString ip;
    QString port;
    WsAuth(){};

    QString getType() override {return "WsAuth";};
    bool fromQJsonObject(const QJsonObject &) override;
    QJsonObject toQJsonObject() override;

};

#endif //DEMO02_DATACLASSES_H
