//
// Created by no3core on 2023/8/25.
//




#include "../../service/MessageService.h"

using namespace std;

int main(){
//    BaseDao* dao = new BaseDao()
//    MessageService service( )
//    BaseDao testdao = BaseDao("E:\\programming\\Qt\\IMback\\test\\dbfortest.db");
    auto service = MessageService(
            new BaseDao("E:\\programming\\Qt\\IMback\\test\\dbfortest.db")
    );

    // test received message
    Message mockFromMessage;
    mockFromMessage.type = "text";
    mockFromMessage.content = "这是一段测试文本11";
    mockFromMessage.gid  = 2023;
    mockFromMessage.uid   = 5;

    int message_id = 0;
    service.StoreMessage(mockFromMessage,message_id);
    mockFromMessage.id = message_id;

    service.FillMessageFromDB(mockFromMessage);


    // test get message list by time
    auto MessageJson = mockFromMessage.toQJsonObject();
    auto messagelist = service.GetMessagelistByTime(2,2023);
    for (auto msg:messagelist) {
        cout << msg << std::endl;
    }

    // test get group user list
    auto userlist = service.GetGroupUserList(2023);
    for (auto uid:userlist){
        cout<< uid << std::endl;
    }
}