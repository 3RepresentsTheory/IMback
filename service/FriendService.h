//
// Created by 28443 on 2023/8/24.
//

#ifndef DEMO02_FRIENDSERVICE_H
#define DEMO02_FRIENDSERVICE_H
#include "../dao/BaseDao.h"

class FriendService {
private:
    BaseDao* baseDao;
public:
    FriendService();
    ~FriendService();
    bool insertRequest(const string& targetId,const string& id,const string& text);
    map<string ,string > seletInfoByRequestId(const string& requestId);
    bool acceptRequest (const string&userId,const string& friendId,const string& requestId);
    vector<map<string ,string >> getRequests(const string& userId,const string& lastId);
};


#endif //DEMO02_FRIENDSERVICE_H
