//
// Created by 28443 on 2023/8/24.
//

#ifndef DEMO02_USERSERVICE_H
#define DEMO02_USERSERVICE_H
#include "../dao/BaseDao.h"
#include "../apis/UserApi.h"
class User;
class UserService {
private:
    BaseDao* baseDao;
public:
    UserService();
    ~UserService();
    bool insertUser(const User& user);
    User selectUserInfoByName(const std::string& username);
    int selectIdByName(const std::string& username);
    bool isUsernameExists(const std::string& username);
    bool isNicknameExists(const std::string& nickname);
    User validateUserCredentials(const string& username, const string& password);
    bool updateInfo(const int& id,const string &nickname, const string &color, const string &avatar);
    vector<User> getUserInfos(const string& uidParams);

};


#endif //DEMO02_USERSERVICE_H
