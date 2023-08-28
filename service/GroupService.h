//
// Created by no3core on 2023/8/27.
//

#ifndef DEMO02_GROUPSERVICE_H
#define DEMO02_GROUPSERVICE_H


#include <QtCore>
#include "../dao/BaseDao.h"
#include "../dao/DataClasses.h"

class GroupService{
private:
    BaseDao* baseDao;
public:
    GroupService();
    ~GroupService();

    bool CreateGroup(const Group& group,int &last_insert_id);
    bool JoinGroup(qint64 uid, qint64 gid);
    QJsonArray GetGroupHasjoin(qint64 uid,qint64 ddl);
    QJsonObject GetGroupHasjoin(qint64 gid);
};

#endif //DEMO02_GROUPSERVICE_H
