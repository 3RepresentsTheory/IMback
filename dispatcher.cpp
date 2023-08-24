#include <QtCore/QCoreApplication>
#include <QtHttpServer/QHttpServer>
#include "httpServerHeaders/userapi.h"
#define PORT 49425


void userRouting(QHttpServer &HttpServer, UserApi &userApi);


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

    SessionApi* sessionApi = new SessionApi();
    // add global session management

    UserApi userApi(new UserService(),sessionApi);
    // Setup QHttpServer for normal transaction
    QHttpServer httpServer;
    // route request
    userRouting(httpServer, userApi);

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


void userRouting(QHttpServer &HttpServer, UserApi &userApi){
    HttpServer.route(
            "/",
            []() {
                return "Qt Colorpalette example server. Please see documentation for API description";
            }
    );
    // User module
    HttpServer.route(
            "/user/login", QHttpServerRequest::Method::Post,
            [&userApi](const QHttpServerRequest &request) {
                return userApi.login(request);
            }
    );

    HttpServer.route(
            "/user/register", QHttpServerRequest::Method::Post,
            [&userApi](const QHttpServerRequest &request) {
                return userApi.registerSession(request);
            }
    );

    HttpServer.route(
            "/user/info", QHttpServerRequest::Method::Put,
            [&userApi](const QHttpServerRequest &request) {
                return userApi.info(request);
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