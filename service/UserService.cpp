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

bool UserService::insertUser(const User& user) {
    string sql = "INSERT INTO user(username, password, nickname,avatar,color) VALUES (?, ?, ?,?,?);";
    bool success = baseDao->executeUpdate(sql, user.username, user.password, user.nickname,user.avatar,user.color);
    return success;
}

User UserService::selectUserInfoByName(const string &username) {
    vector<map<string, string>> rc;
    string sql = "SELECT id,username, password, nickname,avatar,color FROM user WHERE username = ?;";
    rc = baseDao->executeQuery(sql,username);
    return User(rc[0]);
}

int UserService::selectIdByName(const string &username) {
    vector<map<string, string>> rc;
    string sql = "SELECT id FROM user WHERE username = ?;";
    rc = baseDao->executeQuery(sql,username);
    if(rc[0].empty()){
        return -1;
    }
    return stoi(rc[0]["id"]);
}

bool UserService::isUsernameExists(const string &username) {
    vector<map<string, string>> rc;
    string sql = "SELECT COUNT(*) FROM user WHERE username = ?;";
    rc = baseDao->executeQuery(sql, username);
    if(rc[0]["COUNT(*)"]=="0"){
        return false;
    }
    return true;
}

bool UserService::isNicknameExists(const string &nickname) {
    vector<map<string, string>> rc;
    string sql = "SELECT COUNT(*) FROM user WHERE nickname = ?;";
    rc = baseDao->executeQuery(sql, nickname);
    if(rc[0]["COUNT(*)"]=="0"){
        return false;
    }
    return true;
}

// need handle null here
User UserService::validateUserCredentials(const string &username, const string &password) {
    vector<map<string, string>> rc;
    map<string ,string> rcMap;
    string sql = "SELECT id,username, password, nickname FROM user WHERE username = ? AND password = ?;";
    rc = baseDao->executeQuery(sql,username,password);
    if(rc.empty())
        return User();
    return User(rc[0]);
}

bool UserService::updateInfo(const int &id,const string &nickname, const string &color,const string& avatar){
    string sql = "UPDATE user SET nickname = CASE WHEN ? = '' THEN nickname ELSE ? END, color = CASE WHEN ? = '' THEN color ELSE ? END,avatar = CASE WHEN ? = '' THEN avatar ELSE ? END WHERE id = ?;";
    bool rc = baseDao->executeUpdate(sql,nickname,nickname,color,color,avatar,avatar,to_string(id));
    return rc;
}

vector<User> UserService::getUserInfos(const string& uidParams){
    vector<map<string, string>> rc;
    string sql = "SELECT id,username, password, nickname,avatar,color FROM user WHERE id in ("+uidParams+");";
    rc = baseDao->executeQuery(sql,uidParams);
    vector<User> userInfos;

    for (auto& row : rc) {
        User user(row);
        userInfos.push_back(user);
    }
    return userInfos;
}

