// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "chatserver.h"
#include "../apis/SessionApi.h"

#include <QtWebSockets>
#include <QtCore>

#include <cstdio>
using namespace std;

QT_USE_NAMESPACE

static QString getIdentifier(QWebSocket *peer)
{
    return QStringLiteral("%1:%2").arg(QHostAddress(peer->peerAddress().toIPv4Address()).toString(),
                                       QString::number(peer->peerPort()));
}


void ChatBroadcastServer::closeWaitWsocket(QWebSocket *wsocket,QString errorMsg) {
    // maybe the wsocket has been released
    QTimer* timer = m_socketTimers.value(wsocket);
    if (timer) {
        m_socketTimers.remove(wsocket);
        timer->stop();
        timer->deleteLater();

        wsocket->sendTextMessage(errorMsg);
        wsocket->close();
        wsocket->deleteLater();  // 安全删除 WebSocket 对象
    }

}

ChatBroadcastServer::ChatBroadcastServer(QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(
            new QWebSocketServer(
                    QStringLiteral("Chat Server"),
                    QWebSocketServer::NonSecureMode,
                    this
            )
    )
{}

void ChatBroadcastServer::listen(QHostAddress hostaddr, quint64 port) {
    if (m_pWebSocketServer->listen(hostaddr, port))
    {
        QTextStream(stdout) << "Chat Server listening on port " << port << '\n';

        // add handler for new connection
        connect(
                m_pWebSocketServer,
                &QWebSocketServer::newConnection,
                this,
                &ChatBroadcastServer::onNewConnection
        );
    }
}

ChatBroadcastServer::~ChatBroadcastServer()
{
    m_pWebSocketServer->close();
}

void ChatBroadcastServer::onNewConnection()
{
    auto pSocket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(pSocket) << " connected!\n";
    pSocket->setParent(this);


    // config timeout for listener auth
    QTimer * timer = new QTimer(this);
    m_socketTimers[pSocket] = timer;
    timer->setSingleShot(true);
    timer->start(AUTHTIMEOUT);

    connect(
            timer,
            &QTimer::timeout,
            this,
            [this,pSocket](){
                closeWaitWsocket(pSocket,"Waiting too long for token.Access denied");
            }
    );

    // when received listener token
    connect(
            pSocket,
            &QWebSocket::textMessageReceived,
            this,
            &ChatBroadcastServer::onUpgradeToSocketAuth
    );

    // when listener disconnect
    connect(
            pSocket,
            &QWebSocket::disconnected,
            this,
            &ChatBroadcastServer::socketDisconnected
    );
}


void ChatBroadcastServer::onUpgradeToSocketAuth(const QString &message) {
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    qDebug() << "get the raw data: from message"<< message;

    WsAuth wsauth;
    auto authMsgJson = byteArrayToJsonObject(QByteArray(message.toUtf8()));
    if(!authMsgJson || !wsauth.fromQJsonObject(authMsgJson.value())){
        QTextStream(stdout) << getIdentifier(pSender) << " auth form error\n";
        closeWaitWsocket(pSender,"The form of auth error");
    }
    qDebug() << "get the cookie: "<<wsauth.cookie ;

    auto uid = SessionApi::getInstance()->getIdByCookie(QUuid::fromString(wsauth.cookie));

    if(uid.has_value()){
        // if auth passed: stop timer and add to broadcast list
        m_socketTimers[pSender]->stop();
        m_socketTimers[pSender]->deleteLater();
        m_socketTimers.remove(pSender);
        // add to broadcast list
        socketSession.insert(uid.value(),pSender);
        emit userLogin(QString::fromStdString(to_string(uid.value())),wsauth.ip+':'+wsauth.port);
//        m_clients <<  pSender;
    }else{
        QTextStream(stdout) << getIdentifier(pSender) << " not pass auth!\n";
        closeWaitWsocket(pSender,"Connection authentication failed. Access denied.");
    }

}


void ChatBroadcastServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
    if (pClient)
    {
        auto uid = socketSession.getIdByWs(pClient);
        // perhaps they haven't add to the client list
        if(uid.has_value()){
            if(socketSession.getWsById(uid.value())->size()==1){
                userLogout(uid.value());
            }
            socketSession.remove(pClient);
        }
//        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

//void ChatBroadcastServer::testBroadCast() {
//    for (QWebSocket *pClient : std::as_const(m_clients)) {
//            pClient->sendTextMessage("broadcast message");
//    }
//}
//
//void ChatBroadcastServer::testOnline() {
//    QTextStream(stdout) <<"current online member:\n";
//    for (QWebSocket *pClient : std::as_const(m_clients)) {
//        QTextStream(stdout) << getIdentifier(pClient) << "is online!\n";
//    }
//}

void ChatBroadcastServer::onNeedToBroadCast(MsgLoad data, QVector<qint64> glist) {
    Jsonable*load = data.loadcontent;

    QJsonDocument doc(QJsonObject{
            {"type",load->getType()},
            {"data",load->toQJsonObject()}
    });
    data.FreeContent();

    QByteArray bytes = doc.toJson();
//    qint64 sender_id = msg.uid;

    for(auto uid:glist){
        auto uws = socketSession.getWsById(uid);
//        if(uws.has_value()&&uid!=sender_id)
        if(uws.has_value())
            for (auto ws:uws.value()) {
                ws->sendTextMessage(bytes);
            }
    }
}

void SocketSession::insert(qint64 id, QWebSocket *ws) {
    id2ws.insert(id,ws);
    ws2id.insert(ws,id);
}

void SocketSession::remove(QWebSocket *ws) {
    qint64 id = ws2id.value(ws);
    // more than one client login
    if(id2ws.values(id).size()>1){
        id2ws.remove(id,ws);
    } else{
        id2ws.remove(id);
        ws2id.remove(ws);
    }
}

std::optional<QList<QWebSocket *>> SocketSession::getWsById(qint64 id) {
    if(id2ws.contains(id))
        return id2ws.values(id);
    else
        return std::nullopt;
}

std::optional<qint64> SocketSession::getIdByWs(QWebSocket *ws) {
    if(ws2id.contains(ws))
        return ws2id.value(ws);
    else
        return std::nullopt;
}

//QVector<qint64> SocketSession::allOnlineUsers() {
//    qDebug() << "all online users:\n";
//    QVector<qint64> ret;
//    QMap<id_t,wspt >::const_iterator it;
//    for (it = id2ws.constBegin(); it != id2ws.constEnd(); ++it) {
//        ret.append(it.key());
//        qDebug() << it.key() << "with ws: " << it.value() << "\n";
//    }
//    return ret;
//}
