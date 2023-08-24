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

/*
 * ClientListener : add websocket listener
 * need to first connect the signals to self defined slots,
 * then register a listener with auth token,
 *
 * use case:
 *
 * ClientListener clientlistener(
 *      handleOpenFunc,
 *      handleCloseFunc,
 *      handleMessageFunc,
 *      handleErrorFunc,
 * );
 * clientlistener.registerListener(token)
 *
 */

class ClientListener: public QObject
{
    Q_OBJECT
public:
    explicit ClientListener(
            const std::function<void()>& onOpen,
            const std::function<void()>& onClose,
            const std::function<void(const QString&)>& onMessage,
            const std::function<void(QAbstractSocket::SocketError)>& onError);
    void registerListner(QString authtoken);
signals:
    void CLError(QAbstractSocket::SocketError error){onError(error);};

private:
    QWebSocket*websocket;
    const std::function<void(QAbstractSocket::SocketError)>& onError;
};


#endif //IMBACK_CLIENTLISTEN_H
