// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include <QtCore/QCoreApplication>

#include <QHttpServer>
#include <QHttpServerResponse>


#include <QCoreApplication>
#include <QFile>
#include <QJsonObject>
#include <QString>
#include <QtConcurrent/QtConcurrent>
#include "chatserver.h"

using namespace Qt::StringLiterals;
static inline QString host(const QHttpServerRequest &request)
{
    return QString::fromLatin1(request.value("Host"));
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ChatBroadcastServer server(1235);
    QHttpServer httpServer;
    httpServer.route(
            "/broadcast", [&server]() {
                server.testBroadCast();
                return QHttpServerResponse("text/plain", "Success\n");
            }
    );

    httpServer.route(
            "/checkonline", [&server]() {
                server.testOnline();
                return QHttpServerResponse("text/plain", "Success\n");
            }
    );
//    httpServer.route(
//            "/query",
//            [] (const QHttpServerRequest &request) {
//                QThread* currentThread = QThread::currentThread();
//                qDebug() << "Current Thread ID: " << currentThread->currentThreadId();
//                qDebug() << "Current Thread Name: " << currentThread->objectName();
//                return host(request) + u"/query/"_s;
//            }
//    );
//    httpServer.route("/json/", [] {
//        QThread* currentThread = QThread::currentThread();
//        qDebug() << "Current Thread ID: " << currentThread->currentThreadId();
//        qDebug() << "Current Thread Name: " << currentThread->objectName();
//
//        return QJsonObject{
//                {
//                        {"key1", "1"},
//                        {"key2", "2"},
//                        {"key3", "3"}
//                }
//        };
//    });
//
//    httpServer.route("/assets/<arg>", [] (const QUrl &url) {
//        return QHttpServerResponse::fromFile(u":/assets/"_s + url.path());
//    });
//
//    httpServer.route("/remote_address", [](const QHttpServerRequest &request) {
//        return request.remoteAddress().toString();
//    });
//
//    // Basic authentication example (RFC 7617)
//    httpServer.route("/auth", [](const QHttpServerRequest &request) {
//        auto auth = request.value("authorization").simplified();
//
//        if (auth.size() > 6 && auth.first(6).toLower() == "basic ") {
//            auto token = auth.sliced(6);
//            auto userPass = QByteArray::fromBase64(token);
//
//            if (auto colon = userPass.indexOf(':'); colon > 0) {
//                auto userId = userPass.first(colon);
//                auto password = userPass.sliced(colon + 1);
//
//                if (userId == "Aladdin" && password == "open sesame")
//                    return QHttpServerResponse("text/plain", "Success\n");
//            }
//        }
//        QHttpServerResponse response("text/plain", "Authentication required\n",
//                                     QHttpServerResponse::StatusCode::Unauthorized);
//        response.setHeader("WWW-Authenticate", R"(Basic realm="Simple example", charset="UTF-8")");
//        return response;
//    });

    //! [Using afterRequest()]
    httpServer.afterRequest([](QHttpServerResponse &&resp) {
        resp.setHeader("Server", "Qt HTTP Server");
        return std::move(resp);
    });
    //! [Using afterRequest()]



    const auto port = httpServer.listen(QHostAddress::Any,1234);
    if (!port) {
        qWarning() << QCoreApplication::translate("QHttpServerExample",
                                                  "Server failed to listen on a port.");
        return -1;
    }
    qInfo().noquote()
        << QCoreApplication::translate("QHttpServerExample",
                                       "Running on http://127.0.0.1:%1/"
                                       "(Press CTRL+C to quit)").arg(port);
    return a.exec();
}
