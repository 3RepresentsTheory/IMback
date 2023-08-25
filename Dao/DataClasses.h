//
// Created by no3core on 2023/8/25.
//

#ifndef DEMO02_DATACLASSES_H
#define DEMO02_DATACLASSES_H

#include <QJsonObject>
#include "../HttpServerHeaders/utils.h"

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

#endif //DEMO02_DATACLASSES_H
