//
// Created by 28443 on 2023/8/27.
//

#ifndef DEMO02_MYSQLBASEDAO_H
#define DEMO02_MYSQLBASEDAO_H
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QDebug>
using namespace std;

class BaseDao {
public:
    BaseDao();
    ~BaseDao();
    static QSqlDatabase& getConnection();
    void closeConnection();

    template<typename... Args>
    vector<map<string, string>> executeQuery(const string& sql, const Args&... args) {
        QSqlDatabase db = getConnection();
        QSqlQuery query = QSqlQuery(db);
        query.prepare(QString::fromStdString(sql));
        QVariantList params;
        (params << ... << QVariant(QString::fromStdString(args)));
        for (int i = 0; i < params.size(); ++i) {
            query.bindValue(i, params[i]);
        }

        if (!query.exec()) {
            qDebug() << "Failed to execute query: " << query.lastError();
            return {};
        }

        vector<map<string, string>> results;
        while (query.next()) {
            QSqlRecord record = query.record();
            map<string, string> row;
            for (int i = 0; i < record.count(); ++i) {
                row[record.fieldName(i).toStdString()] = record.value(i).toString().toStdString();
            }
            results.push_back(std::move(row));
        }
        return results;
    }

    template<typename... Args>
    bool executeUpdate(const string& sql, const Args&... args) {
        QSqlDatabase db = getConnection();

        QSqlQuery query = QSqlQuery(db);
        query.prepare(QString::fromStdString(sql));


        QVariantList params;
        (params << ... << QVariant(QString::fromStdString(args)));
        for (int i = 0; i < params.size(); ++i) {
            query.bindValue(i, params[i]);
        }

        bool success = query.exec();
        if (!success) {
            qDebug() << "Failed to execute query: " << query.lastError();
        }

        return success;
    }

};




#endif //DEMO02_MYSQLBASEDAO_H
