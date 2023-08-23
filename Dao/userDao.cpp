#include "userDao.h"

using namespace std;


UserDao::UserDao() {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}
UserDao::~UserDao() {
    if (db) {
        sqlite3_close(db);
    }
}

bool UserDao::insertUser(const string &username, const string &password, const string &nickname) {
    std::string query = "INSERT INTO user (username, password, nickname) VALUES (?, ?, ?);";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, nickname.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

int UserDao::selectIdByName(const std::string& username) {
    std::string query = "SELECT id FROM user WHERE username = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    int userId = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return userId;
}

bool UserDao::isUsernameExists(const std::string& username) {
    std::string query = "SELECT COUNT(*) FROM user WHERE username = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    int count = sqlite3_column_int(stmt, 0);
    bool exists = (count > 0);

    sqlite3_finalize(stmt);
    return exists;
}

bool UserDao::isNicknameExists(const std::string& nickname) {
    std::string query = "SELECT COUNT(*) FROM user WHERE nickname = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, nickname.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    int count = sqlite3_column_int(stmt, 0);
    bool exists = (count > 0);

    sqlite3_finalize(stmt);
    return exists;
}


UserCredentials UserDao::validateUserCredentials(const string& username, const string& password) {
    string query = "SELECT username, password, nickname, id FROM user WHERE username = ? AND password = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return UserCredentials(false, "", "", "", -1);
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return UserCredentials(false, "", "", "", -1);
    }

    string validatedUsername = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    string validatedPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    string validatedNickname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    int validatedId = sqlite3_column_int(stmt, 3);

    sqlite3_finalize(stmt);
    return UserCredentials(true, validatedUsername, validatedPassword, validatedNickname, validatedId);
}

bool UserDao::deleteUser(const std::string &username) {
    std::string query = "DELETE FROM user WHERE username = ?;";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool UserDao::updateUserNickname(const std::string &username, const std::string &newNickname) {
    std::string query = "UPDATE user SET nickname = ? WHERE username = ?;";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, newNickname.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

string UserDao::dbPath = "C:/Users/28443/Desktop/qt/test.db";