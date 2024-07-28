//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_MESSAGEAPI_H
#define DEMO02_MESSAGEAPI_H



#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include "../service/MessageService.h"
#include "../utils/utils.h"
#include "../dao/BaseDao.h"
#include "../dao/DataClasses.h"
#include "SessionApi.h"


#define MAX_HISTORY_RETRIEVE 99

class MessageApi :public QObject{
    Q_OBJECT
public:
    MessageApi(MessageService*msgservice):
        messageService(msgservice){};
    QHttpServerResponse handleSentMessageRequest(const QHttpServerRequest &request);
    QHttpServerResponse retrieveHistoryMsgList(const QHttpServerRequest &request);
private:
    MessageService*messageService;
signals:
    void passMessageToBroadCast(MsgLoad msg,QVector<qint64>glist);
};


#endif //DEMO02_MESSAGEAPI_H
