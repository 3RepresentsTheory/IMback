//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_MESSAGEAPI_H
#define DEMO02_MESSAGEAPI_H


#include "../service/messageService.h"
#include "utils.h"
#include "../Dao/BaseDao.h"
#include "../Dao/DataClasses.h"


class MessageApi {
public:
    MessageApi(){};
    QHttpServerResponse handleSentMessageRequest(const QHttpServerRequest &request);
private:
    bool broadcastMessageToGroup(Message message);
    MessageService service;
};


#endif //DEMO02_MESSAGEAPI_H
