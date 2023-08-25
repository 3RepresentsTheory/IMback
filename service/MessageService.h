//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_MESSAGESERVICE_H
#define DEMO02_MESSAGESERVICE_H


#include <QtTypes>
#include <QVector>
#include "../Dao/BaseDao.h"
#include "../Dao/DataClasses.h"

class MessageService {
private:
    BaseDao* baseDao;
    //Chatserver *broadcastserver
public:
    MessageService(BaseDao* dao): baseDao(dao){};
    ~MessageService();

    bool StoreMessage(const Message& message ,int &last_insert_id);
    void FillMessageFromDB(Message&original_message);
    QVector<Message> GetMessagelistByTime(qint64 mid,qint64 gid);

    QVector<qint64> GetGroupUserList(qint64 gid);

};


#endif //DEMO02_MESSAGESERVICE_H
