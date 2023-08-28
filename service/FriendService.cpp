//
// Created by 28443 on 2023/8/24.
//

#include "FriendService.h"

FriendService::FriendService() {
    baseDao = new BaseDao();
}

FriendService::~FriendService() {
    if(baseDao){
       delete(baseDao);
       baseDao = nullptr;
    }
}

bool FriendService::insertRequest(const string &targetId, const string &id, const string &text) {
    string sql = "INSERT INTO friendRequest(userId,requestUserId,text) VALUES (?,?,?);";
    bool rc=baseDao->executeUpdate(sql,targetId,id,text);
    return rc;
}

map<string ,string > FriendService::seletInfoByRequestId(const string &requestId) {
    vector<map<string,string>> rc;
    string sql = "SELECT userId,requestUserId FROM friendRequest WHERE id = ? AND status = 0;";
    rc = baseDao->executeQuery(sql,requestId);
    if(rc.empty()){
        return {};
    }
    return rc[0];
}

bool FriendService::acceptRequest(const string &userId, const string &friendId,const string& requestId) {
    string friendSql = "INSERT INTO friend(userId,friendId) VALUES (?,?);";
    bool friendSqlrc;
    if(userId<friendId)
        friendSqlrc = baseDao->executeUpdate(friendSql, userId, friendId);
    else
        friendSqlrc = baseDao->executeUpdate(friendSql, friendId, userId);
    string requestSql = "UPDATE friendRequest SET status = 1 WHERE id = ? OR (userId = ? AND requestUserId = ?);";
    bool requestSqlrc = baseDao->executeUpdate(requestSql,requestId,friendId,userId);
    if(friendSqlrc&&requestSqlrc){
        return true;
    }
    return false;
}

vector<map<string ,string >> FriendService::getRequests(const string& userId,const string& lastId){
    string sql = "SELECT id, UNIX_TIMESTAMP(timestamp) AS time, text, requestUserId AS uid FROM friendRequest WHERE userId = ? AND id > ? ;";
    vector<map<string,string>> rc;
    rc = baseDao->executeQuery(sql,userId,lastId);
    return rc;
}

bool FriendService::isFriendExists(const string &userId, const string &friendId) {
    vector<map<string,string>> rc;
    string sql = "SELECT COUNT(*) FROM friend WHERE userId = ? AND friendId = ?;";
    if(userId<friendId)
        rc = baseDao->executeQuery(sql,userId,friendId);
    else
        rc = baseDao->executeQuery(sql,friendId,userId);
    if(rc[0].at("COUNT(*)")=="0")
        return false;
    else
        return true;
}
