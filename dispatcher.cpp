// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "HttpServerHeaders/apibehavior.h"
#include "HttpServerHeaders/types.h"
#include "HttpServerHeaders/utils.h"
#include <QtCore/QCoreApplication>
#include <QtHttpServer/QHttpServer>

#define PORT 49425

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addOptions({
                              { "port", QCoreApplication::translate("main", "The port the server listens on."),
                                "port" },
                      });
    parser.addHelpOption();
    parser.process(app);

    quint16 portArg = PORT;
    if (!parser.value("port").isEmpty())
        portArg = parser.value("port").toUShort();

    auto sessionEntryFactory = std::make_unique<SessionEntryFactory>();
    auto sessions = tryLoadFromFile<SessionEntry>(*sessionEntryFactory, "./sourceFiles/sessions.json");
    SessionApi sessionApi(std::move(sessions), std::move(sessionEntryFactory));

    // Setup QHttpServer
    QHttpServer httpServer;
    httpServer.route("/", []() {
        return "Qt Colorpalette example server. Please see documentation for API description";
    });

    // Login resource
    httpServer.route(
            "/api/login", QHttpServerRequest::Method::Post,
            [&sessionApi](const QHttpServerRequest &request) { return sessionApi.login(request); });

    httpServer.route("/api/register", QHttpServerRequest::Method::Post,
                     [&sessionApi](const QHttpServerRequest &request) {
                         return sessionApi.registerSession(request);
                     });

    httpServer.route("/api/logout", QHttpServerRequest::Method::Post,
                     [&sessionApi](const QHttpServerRequest &request) {
                         return sessionApi.logout(request);
                     });

    const auto port = httpServer.listen(QHostAddress::Any, portArg);
    if (!port) {
        qDebug() << QCoreApplication::translate("QHttpServerExample",
                                                "Server failed to listen on a port.");
        return 0;
    }

    qDebug() << QCoreApplication::translate(
            "QHttpServerExample",
            "Running on http://127.0.0.1:%1/ (Press CTRL+C to quit)")
            .arg(port);

    return app.exec();
}
