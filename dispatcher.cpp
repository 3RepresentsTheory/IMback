#include <QtCore/QCoreApplication>
#include <QtHttpServer/QHttpServer>
#include <QSqlDatabase>
#include "apis/UserApi.h"
#include "apis/FriendApi.h"
#include "apis/MessageApi.h"
#include "apis/GroupApi.h"
#include "broadcast/chatserver.h"
#include "broadcast/BroadCastThread.h"
#include "apis/GroupApi.h"
#include "utils/utils.h"

#define TXPORT 1235
#define BCPORT 1234
// need to modify to server ip
#define HOSTNAME "127.0.0.1"


void userRouting(QHttpServer &HttpServer, UserApi &userApi);
void friendRouting(QHttpServer &HttpServer, FriendApi& friendApi);
void msgRouting(QHttpServer &HttpServer, MessageApi &msgApi);
void groupRouting(QHttpServer &HttpServer, GroupApi &groupApi);
void testingRoutng(QHttpServer &HttpServer);
void fileRouting(QHttpServer &HttpServer);


void broadcastSignalConnecting(
        const ChatBroadcastServer& broadcastServer,
        const MessageApi& messageApi,
        const GroupApi  & groupApi,
        const FriendApi & friendApi
);

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

    MessageService* msgService = new MessageService();
    GroupService * groupService= new GroupService();

    FriendApi friendApi(
            new UserService(),
            new FriendService(),
            groupService,
            msgService);
    MessageApi messageApi(msgService);
    GroupApi groupApi(msgService,groupService);

    // handle messageapi connect to broadcast server&thread
    broadcastSignalConnecting(broadcastServer,messageApi,groupApi,friendApi);

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
    fileRouting(httpServer);

    testingRoutng(httpServer);


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
void broadcastSignalConnecting(
        const ChatBroadcastServer& broadcastServer,
        const MessageApi& messageApi,
        const GroupApi  & groupApi,
        const FriendApi & friendApi
){
    // need to handle send msg event to broadcast
    QObject::connect(
            &messageApi,
            &MessageApi::passMessageToBroadCast,
            &broadcastServer,
            &ChatBroadcastServer::onNeedToBroadCast
    );
    QObject::connect(
            &groupApi,
            &GroupApi::passMessageToBroadCast,
            &broadcastServer,
            &ChatBroadcastServer::onNeedToBroadCast
    );
    QObject::connect(
            &friendApi,
            &FriendApi::passMessageToBroadCast,
            &broadcastServer,
            &ChatBroadcastServer::onNeedToBroadCast
    );

    // broadcast server need to report the status of user online
    QObject::connect(
            &broadcastServer,
            &ChatBroadcastServer::userLogout,
            SessionApi().getInstance(),
            &SessionApi::onUserLogout
    );
    QObject::connect(
            &broadcastServer,
            &ChatBroadcastServer::userLogin,
            SessionApi().getInstance(),
            &SessionApi::onUserLogin
    );

}

void testingRoutng(QHttpServer &HttpServer){
    // I don't know why its has to be in the root ,otherwise the Set-Cookie doesn't work
    HttpServer.route(
            "/",
            []() {
//                return "Qt Colorpalette example server. Please see documentation for API description";
                // send back a chatclient.html in broadcast/chatclient.h
                QFile file("../broadcast/chatclient.html");
                if (!file.open(QIODevice::ReadOnly)) {
                    return QHttpServerResponse(QJsonObject{{"msg","Internal server error"}}, QHttpServerResponder::StatusCode::InternalServerError);
                }
                QByteArray data = file.readAll();
                file.close();
                return QHttpServerResponse(data);
            }
    );

    HttpServer.route(
            "/chatclient.js",
            []() {
//                return "Qt Colorpalette example server. Please see documentation for API description";
                // send back a chatclient.html in broadcast/chatclient.h
                QFile file("../broadcast/chatclient.js");
                if (!file.open(QIODevice::ReadOnly)) {
                    return QHttpServerResponse(QJsonObject{{"msg","Internal server error"}}, QHttpServerResponder::StatusCode::InternalServerError);
                }
                QByteArray data = file.readAll();
                file.close();
                return QHttpServerResponse(data);
            }
    );

    HttpServer.afterRequest([](QHttpServerResponse &&resp,const QHttpServerRequest &req){
        qDebug()<<LogHeader(req).toStdString();
        qDebug()<<resp.statusCode();
        return std::move(resp);
    });
}

void userRouting(QHttpServer &HttpServer, UserApi &userApi){
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

    HttpServer.route(
            "/user/infos", QHttpServerRequest::Method::Post,
            [&userApi](const QHttpServerRequest &request) {
                return userApi.infos(request);
            }
    );

    HttpServer.route(
            "/user/ip", QHttpServerRequest::Method::Get,
            [&userApi](const QHttpServerRequest &request) {
                return userApi.getUserip(request);
            }
    );

    HttpServer.route(
            "/user/onlines", QHttpServerRequest::Method::Post,
            [&userApi](const QHttpServerRequest &request) {
                return userApi.onlines(request);
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

void groupRouting(QHttpServer &HttpServer, GroupApi &groupApi){
    // msgapi use get group list

    HttpServer.route(
            "/group/new",QHttpServerRequest::Method::Post,
            [&groupApi](const QHttpServerRequest&request){
                return groupApi.createGroup(request);
            }
    );

    HttpServer.route(
            "/group/request",QHttpServerRequest::Method::Post,
            [&groupApi](const QHttpServerRequest&request){
                return groupApi.joinGroup(request);
            }
    );

    HttpServer.route(
            "/group/list",QHttpServerRequest::Method::Get,
            [&groupApi](const QHttpServerRequest&request){
                return groupApi.getGroupHasjoin(request);
            }
    );

    HttpServer.route(
            "/group/users",QHttpServerRequest::Method::Get,
            [&groupApi](const QHttpServerRequest&request){
                return groupApi.getGroupUserList(request);
            }
    );

}

void fileRouting(QHttpServer &HttpServer){
    HttpServer.route("/upload",QHttpServerRequest::Method::Post,
        [](const QHttpServerRequest&request){
            for (auto pair : request.headers()) {
                if(pair.first=="Content-Type"
                && pair.second.mid(0,pair.second.indexOf(';'))!="multipart/form-data"
                ){
                    return QHttpServerResponse(QJsonObject{{"msg","请求类型错误"}},
                                               QHttpServerResponse::StatusCode::BadRequest);
                }
            }

            // resolve the body ( why qt not suppport multipart form resolve...
            QByteArray multipartbody = request.body();
            QString    boundary;
            int boundary_length = multipartbody.indexOf("\r\n")+2;
            boundary = QString::fromUtf8(multipartbody.mid(0,boundary_length));
            multipartbody.remove(0,boundary_length);

            boundary_length += 2;
            qint64 firstStart   = 0;
            qint64 firstFileEnd =  multipartbody.indexOf("\r\n"+boundary.toStdString());
            qint64 secondStart  =  firstFileEnd + boundary_length;
            boundary.chop(2);// delete two \r\n
            qint64 secondFileEnd=  multipartbody.indexOf("\r\n"+boundary.toStdString(),secondStart);

            firstStart = multipartbody.indexOf("\r\n", firstStart)+2;
            firstStart = multipartbody.indexOf("\r\n", firstStart)+2;
            firstStart = multipartbody.indexOf("\r\n", firstStart)+2;

            QByteArray filebody   = multipartbody.mid(firstStart, firstFileEnd - firstStart);

            secondStart           = multipartbody.indexOf("\r\n",secondStart)+2;
            secondStart           = multipartbody.indexOf("\r\n",secondStart)+2;

            QByteArray suffixname = multipartbody.mid(secondStart,secondFileEnd-secondStart);

            std::cout<< filebody.size() <<" and "<<suffixname.size()<<std::endl;


            // 生成文件名
            QByteArray fileNameHash = QCryptographicHash::hash(filebody, QCryptographicHash::Md5);
            QString fileName = fileNameHash.toHex() + suffixname;

            // 存储文件
            QString storagePath = QDir::currentPath() + "/uploads/" + fileName;
            QFile file(storagePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(filebody);
                file.close();
                // 构造文件映射的URL
                QString fileUrl = "http://" HOSTNAME ":1235/files/" + fileName;
                return QHttpServerResponse(QJsonObject{{"url",fileUrl}});
            } else {
                return QHttpServerResponse(QJsonObject{{"msg","上传失败"}},QHttpServerResponse::StatusCode::InternalServerError);
            }
    });

    HttpServer.route("/files/",QHttpServerRequest::Method::Get,
        [](QString filename,const QHttpServerRequest&request) {
            QFile file("./uploads/"+filename);
            if (!file.open(QIODevice::ReadOnly)) {
                return QHttpServerResponse(QJsonObject{{"msg", "服务器内部错误"}},
                                           QHttpServerResponder::StatusCode::InternalServerError);
            }
            QByteArray data = file.readAll();
            file.close();
            return QHttpServerResponse(data);
        }
    );
}

