//
// Created by no3core on 2023/8/24.
//



#include <string>
#include <iostream>
#include "../../sqlite/sqlite3.h"
#include <unistd.h>
#include <thread>
#include <map>
#include <vector>
#include <mutex>

using namespace std;


std::mutex DaoLock;
class BaseDao {
private:
    static string dbPath;
public:
    BaseDao(){};
    ~BaseDao(){};
    sqlite3* getConnection(){
        DaoLock.lock();
        sqlite3 * db = nullptr;
        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc != SQLITE_OK) {
            DaoLock.unlock();
            cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
            db = nullptr;
        }
        return db;
    }
    void closeConnection(sqlite3 *db){
        if (db) {
            sqlite3_close(db);
            DaoLock.unlock();
        }
    }

    template<typename... Args>
    bool executeUpdate(const string &sql, const Args&...args){
        sqlite3 *db = getConnection();
        if (!db) {
            return false;
        }

        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            DaoLock.unlock();
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        int index = 1;
        (sqlite3_bind_text(stmt, index++, (args).c_str(), -1, SQLITE_STATIC), ...);  // 展开参数包
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            DaoLock.unlock();
            cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
        sqlite3_finalize(stmt);
        closeConnection(db);
        return true;
    }

    template<typename... Args>
    bool executeUpdate(int &last_insert_id,const string &sql, const Args&...args){
        sqlite3 *db = getConnection();
        if (!db) {
            return false;
        }
        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            DaoLock.unlock();
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        int index = 1;
        (sqlite3_bind_text(stmt, index++, (args).c_str(), -1, SQLITE_STATIC), ...);  // 展开参数包
        rc = sqlite3_step(stmt);
        last_insert_id = sqlite3_last_insert_rowid(db);

        if (rc != SQLITE_DONE) {
            DaoLock.unlock();
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
            DaoLock.unlock();
            cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
        closeConnection(db);
        return results;
    }

};

string BaseDao::dbPath = "../../../test.db";


void try_to_read_ts(){
    auto dao = BaseDao();
    string sql = "SELECT * from test_time";
    vector<map<string, string>> ret = dao.executeQuery(sql);
    std::cout<<""<<std::endl;
}



int main(){
//     int threadNums =300000;
//
//     std::vector<pthread_t> threadpool;
//     // 1 创建 threadNums 个线程
//     for(int idx=0; idx < threadNums; ++idx) {
//         pthread_t pid1;
//         pthread_t pid2;
//         pthread_create(&pid2,NULL,try_to_read_ts,NULL);
//         threadpool.emplace_back(pid1);
//         threadpool.emplace_back(pid1);
//     }

     std::cout<<"work in main thread"<<std::endl;


    try_to_read_ts();

//     // 2 终止 threadNums 个线程
//     for(int idx=0; idx < threadNums; ++idx) {
//         pthread_join(threadpool[idx],NULL);
//     }

     std::cout<<"main thread end"<<std::endl;
     return 0;
}