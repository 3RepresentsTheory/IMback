//
// Created by 28443 on 2023/8/24.
//

#include "BaseDao.h"

std::mutex DaoLock;

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

BaseDao::~BaseDao() {}

void initDatabase(){
    BaseDao initialDao;
    string create_user =
    "CREATE TABLE IF NOT EXISTS user(\n"
    "        id INTEGER PRIMARY KEY,\n"
    "        username TEXT,\n"
    "        password TEXT,\n"
    "        nickname TEXT,\n"
    "        color TEXT,\n"
    "        avatar TEXT\n"
    ");\n";

    string create_group_metadata =
    "CREATE TABLE IF NOT EXISTS groupChat (\n"
    "        gcid INTEGER PRIMARY KEY AUTOINCREMENT,\n"
    "        name TEXT,\n"
    "        owner INTEGER,\n"
    "        gctype TEXT,\n"
    "avatar TEXT default(''),\n"
    "            color TEXT default(''),\n"
    "            msgNum INTEGER default(0),\n"
    "            last_msg_id INTEGER default(0),\n"
    "            last_msg_timestamp INTEGER default(1)\n"
    "            --                        timestamp is not zero for searching\n"
    ");\n";

    string create_group_user =
    "CREATE TABLE IF NOT EXISTS groupUser(\n"
    "        id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
    "        gid INTEGER,\n"
    "        uid INTEGER\n"
    ");\n";

    string create_message =
    "CREATE TABLE IF NOT EXISTS message (\n"
    "        id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
    "        type TEXT,\n"
    "        content TEXT,\n"
    "time INTEGER DEFAULT (strftime('%s', 'now')),\n"
    "        uid INTEGER,\n"
    "        mid INTEGER,\n"
    "        gid INTEGER\n"
    ");\n";

    string insert_aux_msg =
    "insert or IGNORE into message (id,type,content,uid,mid,gid)values(0,'pseudo','testuse',0,0,0);\n";

    string create_trigger =
    "CREATE TRIGGER IF NOT EXISTS update_mid\n"
    "AFTER INSERT ON message\n"
    "FOR EACH ROW\n"
    "WHEN NEW.mid IS NULL\n"
    "BEGIN\n"
    "UPDATE groupChat\n"
    "SET msgNum = msgNum + 1,\n"
    "        last_msg_id = NEW.id,\n"
    "        last_msg_timestamp = NEW.time\n"
    "WHERE groupChat.gcid = NEW.gid;\n"

    "UPDATE message\n"
    "SET mid = (\n"
    "        SELECT groupChat.msgNum\n"
    "FROM groupChat\n"
    "WHERE groupChat.gcid = NEW.gid\n"
    ")\n"
    "WHERE id = NEW.id;\n"
    "END;\n";


    if(!initialDao.executeUpdate(create_user))
        cerr<<"create user table failed"<<endl;
    if(!initialDao.executeUpdate(create_group_metadata))
        cerr<<"create groupChat table failed"<<endl;
    if(!initialDao.executeUpdate(create_group_user))
        cerr<<"create groupUser table failed"<<endl;
    if(!initialDao.executeUpdate(create_message))
        cerr<<"create message table failed"<<endl;
    if(!initialDao.executeUpdate(insert_aux_msg))
        cerr<<"insert aux message failed"<<endl;
    if(!initialDao.executeUpdate(create_trigger))
        cerr<<"create trigger failed"<<endl;

}



