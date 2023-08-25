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
//! [constructor]

//! [onNewConnection]
void ChatBroadcastServer::onNewConnection()
{
    auto pSocket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(pSocket) << " connected!\n";
    pSocket->setParent(this);

    connect(pSocket, &QWebSocket::textMessageReceived,
            this, &ChatBroadcastServer::processMessage);
    connect(pSocket, &QWebSocket::disconnected,
            this, &ChatBroadcastServer::socketDisconnected);

    m_clients << pSocket;
}
//! [onNewConnection]

//! [processMessage]
void ChatBroadcastServer::processMessage(const QString &message)
{
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    for (QWebSocket *pClient : std::as_const(m_clients)) {
        if (pClient != pSender) //don't echo message back to sender
            pClient->sendTextMessage(message);
    }
}
//! [processMessage]

//! [socketDisconnected]
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
//! [socketDisconnected]
