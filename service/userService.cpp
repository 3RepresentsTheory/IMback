//
// Created by 28443 on 2023/8/24.
//

#include "userService.h"

UserService::UserService() {
    baseDao = new BaseDao();
}

UserService::~UserService() {
    if(baseDao!= nullptr)
        delete(baseDao);
}

bool UserService::insertUser(const string &username, const string &password, const string &nickname) {
    string sql = "INSERT INTO user(username, password, nickname) VALUES (?, ?, ?);";
    return baseDao->executeUpdate<string>(sql,username,password,nickname);
}

int UserService::selectIdByName(const string &username) {

}

bool UserService::isUsernameExists(const string &username) {
    vector<map<string, string>> rc;
    string sql = "SELECT COUNT(*) FROM user WHERE username = ?;";
    rc = baseDao->executeQuery<string>(sql,username);
    if(rc[0]["COUNT(*)"]=="0"){
        return false;
    }
    return true;
}

bool UserService::isNicknameExists(const string &nickname) {
    vector<map<string, string>> rc;
    string sql = "SELECT COUNT(*) FROM user WHERE nickname = ?;";
    rc = baseDao->executeQuery<string>(sql,nickname);
    if(rc[0]["COUNT(*)"]=="0"){
        return false;
    }
    return true;
}

map<string ,string > UserService::validateUserCredentials(const string &username, const string &password) {
    vector<map<string, string>> rc;
    map<string ,string> rcMap;
    string sql = "SELECT id,nickname FROM user WHERE username = ? AND password = ?;";
    rc = baseDao->executeQuery<string>(sql,username,password);
    if(rc.empty())
        return rcMap;
    return rc[0];
}
