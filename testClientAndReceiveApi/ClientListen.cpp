//
// Created by no3core on 2023/8/23.
//

#include "ClientListen.h"


ClientListen::ClientListen(QNetworkAccessManager *requester) {

    requester = requester;
    websocket = new QWebSocket();

    connect(websocket, &QWebSocket::connected, this, &ClientListen::onConnected);
    connect(websocket, &QWebSocket::disconnected, this, &ClientListen::onDisconnected);
    connect(websocket, &QWebSocket::textMessageReceived, this, &ClientListen::onTextMessageReceived);
    connect(websocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &ClientListen::onError);
}


void ClientListen::registerLisner(QString authtoken) {
    QString serverBroadCastUrl = "ws://" SERVER_URL ":" "BCAST_PORT";

    if(!websocket->isValid()){
        websocket->open(serverBroadCastUrl);
        websocket->sendTextMessage(authtoken);
    }else{
        emit onError(QAbstractSocket::UnknownSocketError);
    }
}
