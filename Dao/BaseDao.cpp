//
// Created by 28443 on 2023/8/24.
//

#include "BaseDao.h"



sqlite3 *BaseDao::getConnection() {
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
    }
}

BaseDao::BaseDao() {

}


BaseDao::~BaseDao() {

}

string BaseDao::dbPath = "../test.db";



