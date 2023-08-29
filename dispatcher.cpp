#include <QtCore/QCoreApplication>
#include <QtHttpServer/QHttpServer>
#include "apis/userapi.h"
#include "apis/friendApi.h"
#include "apis/MessageApi.h"
#include "apis/GroupApi.h"
#include "broadcast/chatserver.h"
#include "broadcast/BroadCastThread.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <thread>

#define TXPORT 1235
#define BCPORT 1234


void userRouting(QHttpServer &HttpServer, UserApi &userApi);
void friendRouting(QHttpServer &HttpServer, FriendApi& friendApi);
void msgRouting(QHttpServer &HttpServer, MessageApi &msgApi);
void groupRouting(QHttpServer &HttpServer, GroupApi &groupApi);

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // parse start up command line port argument
//    QCommandLineParser parser;
//    parser.addOptions(
//       {
//         {"port", QCoreApplication::translate("main", "The port the server listens on."),
//          "port"},
//       }
//    );
//    parser.addHelpOption();
//    parser.process(app);
    quint16 txportArg = TXPORT;
    quint16 bcportArg = BCPORT;
//    if (!parser.value("port").isEmpty())
//        portArg = parser.value("port").toUShort();

    // set up broadcast server
    ChatBroadcastServer broadcastServer;
    BroadCastThread broadcastThread;

    // add global session management
    UserApi userApi(new UserService());
    FriendApi friendApi(new UserService(),new FriendService());

    MessageService* msgService = new MessageService();
    MessageApi messageApi(msgService);
    GroupApi groupApi(msgService,new GroupService());

    // handle messageapi connect to broadcast server&thread
    QObject::connect(
            &messageApi,
            &MessageApi::passMessageToBroadCast,
            &broadcastServer,
            &ChatBroadcastServer::onNeedToBroadCast
    );

    broadcastThread.start();
    broadcastServer.listen(QHostAddress::Any,bcportArg);
    broadcastServer.moveToThread(&broadcastThread);

    // Setup QHttpServer for normal transaction
    QHttpServer httpServer;

    // route request
    userRouting(httpServer, userApi);
    friendRouting(httpServer,friendApi);
    msgRouting(httpServer,messageApi);
    groupRouting(httpServer,groupApi);


    // start server listen
    const auto port = httpServer.listen(QHostAddress::Any, txportArg);
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
//                return "Qt Colorpalette example server. Please see documentation for API description";
                // send back a chatclient.html in broadcast/chatclient.h
                QFile file("../broadcast/chatclient.html");
                if (!file.open(QIODevice::ReadOnly)) {
                    return QHttpServerResponse("Internal server error", QHttpServerResponder::StatusCode::InternalServerError);
                }
                QByteArray data = file.readAll();
                file.close();
                return QHttpServerResponse(data);
            }
    );
    // User module
    HttpServer.route(
            "/user/login", QHttpServerRequest::Method::Post,
            [&userApi](const QHttpServerRequest &request) {
                return QtConcurrent::run([&userApi, &request]() {
                    return userApi.login(request);
                });
            }
    );

    HttpServer.route(
            "/user/register", QHttpServerRequest::Method::Post,
            [&userApi](const QHttpServerRequest &request) {
                return QtConcurrent::run([&userApi, &request]() {
                    return userApi.registerSession(request);
                });
            }
    );

    HttpServer.route(
            "/user/info", QHttpServerRequest::Method::Put,
            [&userApi](const QHttpServerRequest &request) {
                return QtConcurrent::run([&userApi, &request]() {
                    return userApi.info(request);
                });
            }
    );

    HttpServer.route(
            "/user/infos", QHttpServerRequest::Method::Post,
            [&userApi](const QHttpServerRequest &request) {
                return QtConcurrent::run([&userApi, &request]() {
                    return userApi.infos(request);
                });
            }
    );

    HttpServer.route(
            "/user/ip", QHttpServerRequest::Method::Get,
            [&userApi](const QHttpServerRequest &request) {
                return QtConcurrent::run([&userApi, &request]() {
                    return userApi.getUserip(request);
                });
            }
    );

    HttpServer.route(
            "/user/onlines", QHttpServerRequest::Method::Post,
            [&userApi](const QHttpServerRequest &request) {
                return QtConcurrent::run([&userApi,&request]() {
                    return userApi.onlines(request);
                });
            }
    );

    //test multi thread
    HttpServer.route("/feature/", QHttpServerRequest::Method::Get,
                     [](const QHttpServerRequest &request) {
                         Qt::HANDLE threadId = QThread::currentThreadId();
                         qDebug() << "Current main of feature Thread ID: " << threadId;
                         return QtConcurrent::run([]() {
                             Qt::HANDLE threadId = QThread::currentThreadId();
                             qDebug() << "Current feature Thread ID: " << threadId;
                             std::this_thread::sleep_for(std::chrono::seconds(5));
                             return QHttpServerResponse("the future is coming");
                         });
                     });
}

void msgRouting(QHttpServer &HttpServer, MessageApi &msgApi){

    // Message transaction module
    HttpServer.route(
            "/message/send",QHttpServerRequest::Method::Post,
            [&msgApi](const QHttpServerRequest&request) {
                return QtConcurrent::run([&msgApi, &request]() {
                    return msgApi.handleSentMessageRequest(request);
                });
            }
    );

    HttpServer.route(
            "/message/history",QHttpServerRequest::Method::Get,
            [&msgApi](const QHttpServerRequest&request) {
                return QtConcurrent::run([&msgApi, &request]() {
                    return msgApi.retrieveHistoryMsgList(request);
                });
            }
    );

}

void friendRouting(QHttpServer &HttpServer, FriendApi &friendApi){
    // Friend transaction module
    HttpServer.route(
            "/friend/request", QHttpServerRequest::Method::Post,
            [&friendApi](const QHttpServerRequest &request) {
                return QtConcurrent::run([&friendApi, &request]() {
                    return friendApi.request(request);
                });
            }
    );

    HttpServer.route(
            "/friend/accept", QHttpServerRequest::Method::Post,
            [&friendApi](const QHttpServerRequest &request) {
                return QtConcurrent::run([&friendApi, &request]() {
                    return friendApi.accept(request);
                });
            }
    );

    HttpServer.route(
            "/friend/requests", QHttpServerRequest::Method::Get,
            [&friendApi](const QHttpServerRequest &request) {
                return QtConcurrent::run([&friendApi, &request]() {
                    return friendApi.requests(request);
                });
            }
    );
}

void groupRouting(QHttpServer &HttpServer, GroupApi &groupApi){
    // msgapi use get group list

    HttpServer.route(
            "/group/new",QHttpServerRequest::Method::Post,
            [&groupApi](const QHttpServerRequest&request) {
                return QtConcurrent::run([&groupApi, &request]() {
                    return groupApi.createGroup(request);
                });
            }
    );

    HttpServer.route(
            "/group/request",QHttpServerRequest::Method::Post,
            [&groupApi](const QHttpServerRequest&request) {
                return QtConcurrent::run([&groupApi, &request]() {
                    return groupApi.joinGroup(request);
                });
            }
    );

    HttpServer.route(
            "/group/list",QHttpServerRequest::Method::Get,
            [&groupApi](const QHttpServerRequest&request) {
                return QtConcurrent::run([&groupApi, &request]() {
                    return groupApi.getGroupHasjoin(request);
                });
            }
    );

    HttpServer.route(
            "/group/users",QHttpServerRequest::Method::Get,
            [&groupApi](const QHttpServerRequest&request) {
                return QtConcurrent::run([&groupApi, &request]() {
                    return groupApi.getGroupUserList(request);
                });
            }
    );

}