#ifndef CHATBROADCASTSERVER_H
#define CHATBROADCASTSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QTimer>
#include <QMap>
#include "../dao/DataClasses.h"

#define AUTHTIMEOUT 10000

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QString)

//TODO: need consistency maintainess and locks
class SocketSession{
public:
    using id_t = qint64;
    using wspt = QWebSocket*;
    void insert(qint64 id,QWebSocket* ws);
    void remove(QWebSocket* ws);
    std::optional<QList<QWebSocket *>> getWsById(qint64 id);
    std::optional<qint64>      getIdByWs(QWebSocket *ws);

private:
    // one id can only map with one web socket
    QMultiMap<id_t,wspt> id2ws;
    QMap<wspt,id_t> ws2id;
};




class ChatBroadcastServer : public QObject
{
    Q_OBJECT
public:
    explicit ChatBroadcastServer(QObject *parent = nullptr);
    ~ChatBroadcastServer() override;

//    void testBroadCast();
//    void testOnline();
    void listen(QHostAddress,quint64);
    // (uid list) "all" default get all online user, "<uid1>,<uid2>,..." get specific user
//    QVector<qint64> getOnlineUsersList(QString uidlist);

public slots:
    void onNeedToBroadCast(MsgLoad data,QVector<qint64> glist);

signals:
    void userLogout(qint64 uid);
    void userLogin(QString uid, QString ipport);

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

//    QList<QWebSocket *> m_clients;
    SocketSession socketSession;

};

#endif
