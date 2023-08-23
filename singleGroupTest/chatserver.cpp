// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "chatserver.h"

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

//! [constructor]
ChatBroadcastServer::ChatBroadcastServer(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(
            new QWebSocketServer(
                    QStringLiteral("Chat Server"),
                    QWebSocketServer::NonSecureMode,
                    this
            )
    )
{
    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
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
    if(xx){
        // if auth passed: stop timer and add to broadcast list
        m_socketTimers[pSender]->stop();
        m_socketTimers.remove(pSender);
        // add to broadcast list
        m_clients <<  pSender;
    }else{
        QTextStream(stdout) << getIdentifier(pSender) << "not pass auth!\n";
        closeWaitWsocket(pSender,"Connection authentication failed. Access denied.");
    }

//    for (QWebSocket *pClient : std::as_const(m_clients)) {
//        if (pClient != pSender) //don't echo message back to sender
//            pClient->sendTextMessage(message);
//    }
}


void ChatBroadcastServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
    if (pClient)
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}
