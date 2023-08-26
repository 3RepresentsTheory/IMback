//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_MESSAGEAPI_H
#define DEMO02_MESSAGEAPI_H



#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include "../service/MessageService.h"
#include "../Utils/utils.h"
#include "../Dao/BaseDao.h"
#include "../Dao/DataClasses.h"
#include "SessionApi.h"


class MessageApi {
public:
    MessageApi(MessageService*msgservice,SessionApi*sessionapi):
        messageService(msgservice),sessionApi(sessionapi){};
    QHttpServerResponse handleSentMessageRequest(const QHttpServerRequest &request);
    QHttpServerResponse retrieveHistoryMsgList(const QHttpServerRequest &request);
private:
    bool broadcastMessageToGroup(Message message);

    MessageService*messageService;
    SessionApi* sessionApi;
};


#endif //DEMO02_MESSAGEAPI_H
