//
// Created by no3core on 2023/8/23.
//

#include "ClientListen.h"


ClientListener::ClientListener(
        const std::function<void()>& onOpen,
        const std::function<void()>& onClose,
        const std::function<void(const QString&)>& onMessage,
        const std::function<void(QAbstractSocket::SocketError)>& onError
):onError(onError){
    websocket = new QWebSocket();
    connect(websocket,&QWebSocket::connected,this,onOpen);
    connect(websocket,&QWebSocket::disconnected,this,onClose);
    connect(websocket,&QWebSocket::textMessageReceived,this,onMessage);
    connect(websocket,&QWebSocket::errorOccurred,this,onError);
}


void ClientListener::registerListner(QString authtoken) {
    QString serverBroadCastUrl = "ws://" SERVER_URL ":" BCAST_PORT;

    if(!websocket->isValid()){
        websocket->open(serverBroadCastUrl);
        websocket->sendTextMessage(authtoken);
    }else{
        emit CLError(QAbstractSocket::UnknownSocketError);
    }
}
