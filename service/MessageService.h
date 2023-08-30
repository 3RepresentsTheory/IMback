//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_MESSAGESERVICE_H
#define DEMO02_MESSAGESERVICE_H


#include <QtTypes>
#include <QVector>
#include "../dao/BaseDao.h"
#include "../dao/DataClasses.h"

class MessageService {
private:
    BaseDao* baseDao;
    //Chatserver *broadcastserver
public:
    MessageService();
    ~MessageService();

    bool StoreMessage(const Message& message ,int &last_insert_id);
    void FillMessageFromDB(Message&original_message);
    QJsonArray GetMessagelistByPeriod(qint64 gid, qint64 start, qint64 end);

    QVector<qint64> GetGroupUserList(qint64 gid);

};


#endif //DEMO02_MESSAGESERVICE_H
