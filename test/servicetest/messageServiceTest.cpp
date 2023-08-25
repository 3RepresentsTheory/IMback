//
// Created by no3core on 2023/8/25.
//




#include "../../service/messageService.h"
#include "../../Dao/BaseDao.h"

using namespace std;

int main(){
//    BaseDao* dao = new BaseDao()
//    MessageService service( )
    BaseDao testdao = BaseDao("../dbfortest.db");
    auto service = MessageService(&testdao);

    // test received message
    Message mockFromMessage;
    mockFromMessage.type = "text";
    mockFromMessage.content = "这是一段测试文本";
    mockFromMessage.gid  = 2023;
    mockFromMessage.uid   = 5;

    int message_id = 0;
    service.StoreMessage(mockFromMessage,message_id);



}