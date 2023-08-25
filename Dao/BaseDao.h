//
// Created by 28443 on 2023/8/24.
//

#ifndef DEMO02_BASEDAO_H
#define DEMO02_BASEDAO_H
#include <iostream>
#include "../sqlite/sqlite3.h"
#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <mutex>

using namespace std;

// TODO: modify DaoLock to Read-Write Lock
std::mutex DaoLock;


class BaseDao {
private:
    string dbPath;
public:
    explicit BaseDao(string Path = "../test.db"):dbPath(Path){};
    ~BaseDao();
    sqlite3* getConnection();
    void closeConnection(sqlite3 *db);

    template<typename... Args>
    bool executeUpdate(const string &sql, const Args&...args);

    template<typename... Args>
    bool executeUpdate(int& last_insert_id,const string &sql, const Args&...args);

    template<typename... Args>
    vector<map<string, string>> executeQuery(const string &sql, const Args&...args);
};


#endif //DEMO02_BASEDAO_H
