//
// Created by 28443 on 2023/8/23.
//

#ifndef DEMO02_USERDAO_H
#define DEMO02_USERDAO_H
#include <iostream>
#include "../sqlite/sqlite3.h"
#include <string>

using namespace std;

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
};
#endif //DEMO02_USERDAO_H
