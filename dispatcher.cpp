#include "HttpServerHeaders/apibehavior.h"
#include "HttpServerHeaders/types.h"
#include "HttpServerHeaders/utils.h"
#include <QtCore/QCoreApplication>
#include <QtHttpServer/QHttpServer>

#define PORT 49425


void serverRouting(QHttpServer &HttpServer,SessionApi &sessionapi);

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // parse start up command line port argument
    QCommandLineParser parser;
    parser.addOptions(
       {
         {"port", QCoreApplication::translate("main", "The port the server listens on."),
          "port"},
       }
    );
    parser.addHelpOption();
    parser.process(app);
    quint16 portArg = PORT;
    if (!parser.value("port").isEmpty())
        portArg = parser.value("port").toUShort();

    // add global session management
    auto sessionEntryFactory = make_unique<SessionEntryFactory>();
    SessionApi sessionApi(std::move(sessionEntryFactory));

    // Setup QHttpServer for normal transaction
    QHttpServer httpServer;
    // route request
    serverRouting(httpServer,sessionApi);

    // start server listen
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


void serverRouting(QHttpServer &httpServer,SessionApi &sessionApi){
    httpServer.route(
            "/",
            []() {
                return "Qt Colorpalette example server. Please see documentation for API description";
            }
    );
    // User module
    httpServer.route(
            "/user/login", QHttpServerRequest::Method::Post,
            [&sessionApi](const QHttpServerRequest &request) {
                return sessionApi.login(request);
            }
    );

    httpServer.route(
            "/user/register", QHttpServerRequest::Method::Post,
            [&sessionApi](const QHttpServerRequest &request) {
                return sessionApi.registerSession(request);
            }
    );

    httpServer.route(
            "/user/logout", QHttpServerRequest::Method::Post,
            [&sessionApi](const QHttpServerRequest &request) {
                return sessionApi.logout(request);
            }
    );

    // Message transaction module
//    httpServer.route(
//            "/message/send",QHttpServerRequest::Method::Post,
//
//    );
//
//    httpServer.route(
//            "/message/history",QHttpServerRequest::Method::Get,
//
//    );

    // Friend transaction module
}