//
// Created by 28443 on 2023/8/24.
//

#ifndef DEMO02_USERSERVICE_H
#define DEMO02_USERSERVICE_H
#include "../Dao/BaseDao.h"

class UserService {
private:
    BaseDao* baseDao;
public:
    UserService();
    ~UserService();
    bool insertUser(const string &username, const string &password, const string &nickname);
    int selectIdByName(const std::string& username);
    bool isUsernameExists(const std::string& username);
    bool isNicknameExists(const std::string& nickname);
    map<string ,string > validateUserCredentials(const string& username, const string& password);
};


#endif //DEMO02_USERSERVICE_H
