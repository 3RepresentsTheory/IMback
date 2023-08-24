//
// Created by no3core on 2023/8/23.
//



#ifndef IMBACK_CLIENTLISTEN_H
#define IMBACK_CLIENTLISTEN_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QTimer>
#include <QMap>
#include <QNetworkAccessManager>
#include <QWebSocket>

QT_FORWARD_DECLARE_CLASS(QWebSocket)

#define SERVER_URL "127.0.0.1"
#define BCAST_PORT "1235"


class ClientListen:public QObject
{
    Q_OBJECT
public:
    explicit ClientListen(QNetworkAccessManager *requester);
    void registerLisner(QString authtoken);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &message);
    void errorOccurred(QAbstractSocket::SocketError error);

private slots:
    void onConnected(){
        emit connected();
    }
    void onDisconnected(){
        emit disconnected();
    }
    // can modify later to jsonfy or formalize the broadcast message
    void onTextMessageReceived(const QString &message){
        emit messageReceived(message);
    }
    void onError(QAbstractSocket::SocketError error){
        emit errorOccurred(error);
    }
private:
    QWebSocket*websocket;
};


#endif //IMBACK_CLIENTLISTEN_H
