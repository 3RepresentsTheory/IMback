//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_MESSAGESERVICE_H
#define DEMO02_MESSAGESERVICE_H


class MessageService {
private:
    BaseDao* baseDao;
public:
    MessageService();
    ~MessageService();

    bool StoreMessage(const Message& message );

    Message GetMessage(qint32 message_id );
    QVector<Message> GetMessagelistByTime(int timestamp);

};


#endif //DEMO02_MESSAGESERVICE_H
