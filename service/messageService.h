//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_MESSAGESERVICE_H
#define DEMO02_MESSAGESERVICE_H


#include <QtTypes>
#include "../HttpServerHeaders/messageapi.h"
#include "../Dao/BaseDao.h"

class MessageService {
private:
    BaseDao* baseDao;
    //Chatserver *broadcastserver
public:
    MessageService();
    ~MessageService();

    bool StoreMessage(const Message& message );
    Message GetMessage(qint32 message_id );
    QVector<Message> GetMessagelistByTime(int timestamp);

    QVector<qint32> GetGroupUserList(qint32 gid);

    qint32 GetLastInsertId();
};


#endif //DEMO02_MESSAGESERVICE_H
