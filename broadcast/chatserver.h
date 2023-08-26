#ifndef CHATBROADCASTSERVER_H
#define CHATBROADCASTSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QTimer>
#include <QMap>
#include "../dao/DataClasses.h"

#define AUTHTIMEOUT 6000

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QString)

class SocketSession{
public:
    using id_t = qint64;
    using wspt = QWebSocket*;
    void insert(qint64 id,QWebSocket* ws);
    void remove(QWebSocket* ws);
    std::optional<QWebSocket*> getWsById(qint64 id);

private:
    // one id can only map with one web socket
    QMap<id_t,wspt> id2ws;
    QMap<wspt,id_t> ws2id;
};




class ChatBroadcastServer : public QObject
{
    Q_OBJECT
public:
    explicit ChatBroadcastServer(quint16 port, QObject *parent = nullptr);
    ~ChatBroadcastServer() override;

//    void testBroadCast();
//    void testOnline();
    void listen(QHostAddress,quint64);

public slots:
    void onNeedToBroadCast(Message msg,QVector<qint64> glist);

private:
    void closeWaitWsocket(QWebSocket*,QString errorMsg);

private slots:
    void onNewConnection();
    void onUpgradeToSocketAuth(const QString &message);
    void socketDisconnected();


private:
    QWebSocketServer *m_pWebSocketServer;

    // timeout setting for upgrade to socket
    QMap<QWebSocket*, QTimer*> m_socketTimers;

    QList<QWebSocket *> m_clients;
    SocketSession socketSession;

};

#endif
