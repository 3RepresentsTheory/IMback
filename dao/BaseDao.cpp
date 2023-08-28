//
// Created by 28443 on 2023/8/27.
//

#include "BaseDao.h"

BaseDao::BaseDao() {

}

BaseDao::~BaseDao() {
    closeConnection();
}

QSqlDatabase& BaseDao::getConnection() {
    static QSqlDatabase db;
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("172.25.125.23");
        db.setPort(3306);
        db.setDatabaseName("chatRoom");
        db.setUserName("root");
        db.setPassword("1213");
        if (!db.open()) {
            qDebug() << "Failed to connect to database: " << db.lastError();
        }
    }
    return db;
}


void BaseDao::closeConnection() {
    QSqlDatabase db = getConnection();
    if (db.isValid() && db.isOpen()) {
        db.close();
    }
}
