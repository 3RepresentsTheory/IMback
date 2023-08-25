//
// Created by 28443 on 2023/8/23.
//

#ifndef DEMO02_USERDAO_H
#define DEMO02_USERDAO_H
#include <iostream>
#include "../sqlite/sqlite3.h"
#include <string>
#include <tuple>

using namespace std;

using UserCredentials = tuple<bool, string, string, string, int>;

class UserDao {
private:
    sqlite3 *db;
    static string dbPath;
public:
    UserDao();
    ~UserDao();
    bool deleteUser(const string &username);
    bool updateUserNickname(const string &username, const string &newNickname);
    bool insertUser(const string &username, const string &password, const string &nickname);
    int selectIdByName(const std::string& username);
    bool isUsernameExists(const std::string& username);
    bool isNicknameExists(const std::string& nickname);

    UserCredentials validateUserCredentials(const string& username, const string& password);
};
#endif //DEMO02_USERDAO_H
