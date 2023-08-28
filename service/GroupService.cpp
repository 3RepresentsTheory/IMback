//
// Created by no3core on 2023/8/27.
//

#include "GroupService.h"

GroupService::GroupService() {
    baseDao = new BaseDao();
}

GroupService::~GroupService() {
    if(baseDao!= nullptr)
        delete(baseDao);
}

bool GroupService::CreateGroup(const Group &group, int &last_insert_id) {
    string sql = "INSERT INTO groupChat(name, owner, gctype, avatar, color) VALUES (?, ?, ?, ?, ?);";
    return baseDao->executeUpdate(
            last_insert_id,
            sql,
            group.name.toStdString(),
            to_string(group.owner),
            group.type.toStdString(),
            group.avatar.toStdString(),
            group.color.toStdString()
    );
}

bool GroupService::JoinGroup(qint64 uid, qint64 gid) {
    string sql = "INSERT INTO groupUser(uid, gid) VALUES (?, ?);";
    return baseDao->executeUpdate(
            sql,
            to_string(uid),
            to_string(gid)
    );
}

QJsonArray GroupService::GetGroupHasjoin(qint64 uid,qint64 ddl) {
    string sql =
            "SELECT gc.*,m.* FROM ( SELECT * FROM groupChat WHERE last_msg_timestamp > ? ) gc JOIN ( SELECT * FROM groupUser WHERE uid = ? )gu ON gc.gcid = gu.gid JOIN message m ON gc.last_msg_id = m.id";
    // I don't know why the sqlstring cannot be multiple line...
    // this is the sql string which is same to above

    //    string sql =
    //    "SELECT gc.*,m.*"
    //    "FROM ("
    //    "   SELECT *"
    //    "   FROM groupChat"
    //    "   WHERE last_msg_timestamp > ?"
    //    ") gc"
    //    "JOIN ("
    //    "   SELECT *"
    //    "   FROM groupUser"
    //    "   WHERE uid = ?"
    //    ") gu ON gc.gcid = gu.gid"
    //    "JOIN message m ON gc.last_msg_id = m.id";

    vector<map<string,string>>
            ret = baseDao->executeQuery(sql,to_string(ddl), to_string(uid));
    auto groupList = QJsonArray();
    for(auto grp_entry : ret){
        groupList.append(Group(grp_entry, true).toQJsonObjectWithLastMsg());
    }
    return groupList;
}

QJsonObject GroupService::GetGroupHasjoin(qint64 gid) {
    string sql = "SELECT gc.*,m.* FROM groupChat gc,message m WHERE gc.last_msg_id = m.id and gc.gcid = ?";
    vector<map<string,string>>
            ret = baseDao->executeQuery(sql,to_string(gid));
    return Group(ret[0],true).toQJsonObjectWithLastMsg();
}
