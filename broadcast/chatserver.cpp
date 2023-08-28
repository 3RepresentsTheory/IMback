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
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
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
    qDebug() << "get the cookie: "<<message ;
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    auto uid = SessionApi::getInstance()->getIdByCookie(QUuid::fromString(message));
    if(uid.has_value()){
        // if auth passed: stop timer and add to broadcast list
        m_socketTimers[pSender]->stop();
        m_socketTimers[pSender]->deleteLater();
        m_socketTimers.remove(pSender);
        // add to broadcast list
        socketSession.insert(uid.value(),pSender);
//        m_clients <<  pSender;
    }else{
        QTextStream(stdout) << getIdentifier(pSender) << "not pass auth!\n";
        closeWaitWsocket(pSender,"Connection authentication failed. Access denied.");
    }

}


void ChatBroadcastServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
    if (pClient)
    {
        socketSession.remove(pClient);
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
            uws.value()->sendTextMessage(bytes);
    }
}

void SocketSession::insert(qint64 id, QWebSocket *ws) {
    id2ws.insert(id,ws);
    ws2id.insert(ws,id);
}

void SocketSession::remove(QWebSocket *ws) {
    qint64 id = ws2id.value(ws);
    id2ws.remove(id);
    ws2id.remove(ws);
}

std::optional<QWebSocket *> SocketSession::getWsById(qint64 id) {
    if(id2ws.contains(id))
        return id2ws.value(id);
    else
        return std::nullopt;
}

QVector<qint64> SocketSession::allOnlineUsers() {
    qDebug() << "all online users:\n";
    QVector<qint64> ret;
    QMap<id_t,wspt >::const_iterator it;
    for (it = id2ws.constBegin(); it != id2ws.constEnd(); ++it) {
        ret.append(it.key());
        qDebug() << it.key() << "with ws: " << it.value() << "\n";
    }
    return ret;
}
