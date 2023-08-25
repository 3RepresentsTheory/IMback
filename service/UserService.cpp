//
// Created by 28443 on 2023/8/24.
//

#include "UserService.h"

UserService::UserService() {
    baseDao = new BaseDao();
}

UserService::~UserService() {
    if(baseDao!= nullptr) {
        delete (baseDao);
        baseDao = nullptr;
    }
}

bool UserService::insertUser(const string &username, const string &password, const string &nickname) {
    string sql = "INSERT INTO user(username, password, nickname) VALUES (?, ?, ?);";
    return baseDao->executeUpdate<string>(sql,username,password,nickname);
}

int UserService::selectIdByName(const string &username) {
    vector<map<string, string>> rc;
    string sql = "SELECT id FROM user WHERE username = ?;";
    rc = baseDao->executeQuery<string>(sql,username);
    if(rc[0]["id"].empty()){
        return -1;
    }
    return stoi(rc[0]["id"]);
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

bool UserService::updateInfo(const int &id,const string &nickname, const string &color,const string& avatar){
    string sql = "UPDATE user SET nickname = CASE WHEN ? = '' THEN nickname ELSE ? END, color = CASE WHEN ? = '' THEN color ELSE ? END,avatar = CASE WHEN ? = '' THEN avatar ELSE ? END WHERE id = ?;";
    bool rc = baseDao->executeUpdate<string>(sql,nickname,nickname,color,color,avatar,avatar,to_string(id));
    return rc;
}
