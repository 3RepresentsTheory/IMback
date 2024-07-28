//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_SOCIALAPI_H
#define DEMO02_SOCIALAPI_H


#include <QtCore>
#include <QHttpServerResponse>
#include "../service/MessageService.h"
#include "../service/GroupService.h"

class GroupApi : public QObject{
    Q_OBJECT
public:
    GroupApi(
            MessageService*msgservice,
            GroupService*groupservice
            ):
        msgService(msgservice),groupService(groupservice){};
    QHttpServerResponse createGroup(const QHttpServerRequest &request);
    QHttpServerResponse joinGroup(const QHttpServerRequest &request);
    QHttpServerResponse getGroupHasjoin(const QHttpServerRequest &request);
    QHttpServerResponse getGroupUserList(const QHttpServerRequest &request);
private:
    MessageService* msgService;
    GroupService* groupService;

signals:
    void passMessageToBroadCast(MsgLoad msg,QVector<qint64>glist);
};


#endif //DEMO02_SOCIALAPI_H
