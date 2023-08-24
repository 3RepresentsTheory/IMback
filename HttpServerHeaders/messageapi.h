//
// Created by no3core on 2023/8/24.
//

#ifndef DEMO02_MESSAGEAPI_H
#define DEMO02_MESSAGEAPI_H


#include <QHttpServerRequest>
#include <QHttpServerResponse>

class MessageApi {
public:
    MessageApi(){};
    QHttpServerResponse handleSentMessageRequest(const QHttpServerRequest &request);
private:
    int broadcastMessageToGroup(int gid, QString messagetext);
};


#endif //DEMO02_MESSAGEAPI_H
