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

bool UserService::updateInfo(const int &id,const string &nickname, const string &color,const string& avatar){
    string sql1 = "UPDATE user SET nickname = CASE WHEN ? = '' THEN nickname ELSE ? END WHERE id = ?;";
    bool rc1 = baseDao->executeUpdate<string>(sql1,nickname,nickname,to_string(id));
    string sql2 = "UPDATE userDetail SET nickname = CASE WHEN ? = '' THEN nickname ELSE ? END WHERE id = ?;";
    bool rc2 = baseDao->executeUpdate<string>(sql2,nickname,nickname,to_string(id));
    string sql3 = "UPDATE userDetail SET color = CASE WHEN ? = '' THEN color ELSE ? END WHERE id = ?;";
    bool rc3 = baseDao->executeUpdate<string>(sql3,color,color,to_string(id));
    string sql4 = "UPDATE userDetail SET avatar = CASE WHEN ? = '' THEN avatar ELSE ? END WHERE id = ?;";
    bool rc4 = baseDao->executeUpdate<string>(sql4,avatar,avatar,to_string(id));
    if(rc1&&rc2&&rc3&&rc4){
        return true;
    }
    return false;
}
