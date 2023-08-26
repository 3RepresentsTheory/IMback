#ifndef CHATBROADCASTSERVER_H
#define CHATBROADCASTSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QTimer>
#include <QMap>

#define AUTHTIMEOUT 2000

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QString)

class ChatBroadcastServer : public QObject
{
    Q_OBJECT
public:
    explicit ChatBroadcastServer(quint16 port, QObject *parent = nullptr);
    ~ChatBroadcastServer() override;

    void testBroadCast();
    void testOnline();

private:
    void closeWaitWsocket(QWebSocket*,QString errorMsg);

private slots:
    void onNewConnection();
    void onUpgradeToSocketAuth(const QString &message);
    void socketDisconnected();

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;

    // timeout setting for upgrade to socket
    QMap<QWebSocket*, QTimer*> m_socketTimers;

};

#endif
