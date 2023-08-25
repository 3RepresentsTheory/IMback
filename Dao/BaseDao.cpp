//
// Created by 28443 on 2023/8/24.
//

#include "BaseDao.h"

string BaseDao::dbPath = "../test.db";

sqlite3 *BaseDao::getConnection() {
    DaoLock.lock();
    sqlite3 * db = nullptr;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
    return db;
}

void BaseDao::closeConnection(sqlite3 *db) {
    if (db) {
        sqlite3_close(db);
        DaoLock.unlock();
    }
}

template<typename... Args>
bool BaseDao::executeUpdate(const std::string &sql, const Args &...args) {
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
bool BaseDao::executeUpdate(int& last_insert_id,const string &sql, const Args&...args){
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

BaseDao::BaseDao() {}
BaseDao::~BaseDao() {}



