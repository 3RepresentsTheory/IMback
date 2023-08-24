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

using namespace std;

class BaseDao {
private:
    static string dbPath;
public:
    BaseDao();
    ~BaseDao();
    sqlite3* getConnection();
    void closeConnection(sqlite3 *db);


    template<typename... Args>
    bool executeUpdate(const string &sql, const Args&...args){
        sqlite3 *db = getConnection();
        if (!db) {
            return false;
        }

        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        int index = 1;
        (sqlite3_bind_text(stmt, index++, (args).c_str(), -1, SQLITE_STATIC), ...);  // 展开参数包

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        closeConnection(db);
        return true;
    }


    template<typename... Args>
    vector<map<string, string>> executeQuery(const string &sql, const Args&...args){
        vector<map<string, string>> results;
        sqlite3 *db = getConnection();
        if(!db){
            return results;
        }
        sqlite3_stmt *stmt;
        int rc;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            closeConnection(db);
            return {};
        }

        int index = 1;
        (sqlite3_bind_text(stmt, index++, (args).c_str(), -1, SQLITE_STATIC), ...);  // 展开参数包

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            map<string, string> row;
            for (int i = 0; i < sqlite3_column_count(stmt); i++) {
                string columnName = sqlite3_column_name(stmt, i);
                string columnValue = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                row[columnName] = columnValue;
            }
            results.push_back(row);
        }

        if (rc != SQLITE_DONE) {
            cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
        closeConnection(db);
        return results;
    }
};


#endif //DEMO02_BASEDAO_H
