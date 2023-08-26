#include <QtCore/QCoreApplication>
#include <QtHttpServer/QHttpServer>
#include "apis//userapi.h"
#include "apis//friendApi.h"
#include "broadcast/chatserver.h"
#include "apis/MessageApi.h"

#define PORT 49425


void userRouting(QHttpServer &HttpServer, UserApi &userApi);
void friendRouting(QHttpServer &HttpServer, FriendApi& friendApi);

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

    // set up broadcast server
    ChatBroadcastServer broadcastServer(1234);
    QThread broadcastThread;

    // add global session management
    UserApi userApi(new UserService());
    FriendApi friendApi(new UserService(),new FriendService());
    MessageApi messageApi(new MessageService());

    // handle messageapi connect to broadcast server&thread
    QObject::connect(
            &messageApi,
            &MessageApi::passMessageToBroadCast,
            &broadcastServer,
            &ChatBroadcastServer::onNeedToBroadCast
    );

    broadcastThread.start();
    broadcastServer.moveToThread(&broadcastThread);

    // Setup QHttpServer for normal transaction
    QHttpServer httpServer;
    // route request
    userRouting(httpServer, userApi);
    friendRouting(httpServer,friendApi);
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



}
void msgRouting(QHttpServer &HttpServer, MessageApi &msgApi){

    // Message transaction module
    HttpServer.route(
            "/message/send",QHttpServerRequest::Method::Post,
            [&msgApi](const QHttpServerRequest&request){
                return msgApi.handleSentMessageRequest(request);
            }
    );

    HttpServer.route(
            "/message/history",QHttpServerRequest::Method::Get,
            [&msgApi](const QHttpServerRequest&request){
                return msgApi.retrieveHistoryMsgList(request);
            }
    );

}

void friendRouting(QHttpServer &HttpServer, FriendApi &friendApi){
    // Friend transaction module
    HttpServer.route(
            "/friend/request", QHttpServerRequest::Method::Post,
            [&friendApi](const QHttpServerRequest &request) {
                return friendApi.request(request);
            }
    );

    HttpServer.route(
            "/friend/accept", QHttpServerRequest::Method::Post,
            [&friendApi](const QHttpServerRequest &request) {
                return friendApi.accept(request);
            }
    );

    HttpServer.route(
            "/friend/requests", QHttpServerRequest::Method::Get,
            [&friendApi](const QHttpServerRequest &request) {
                return friendApi.requests(request);
            }
    );
}