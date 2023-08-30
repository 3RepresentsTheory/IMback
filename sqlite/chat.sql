-- CREATE TABLE friend (
--                         id INTEGER PRIMARY KEY AUTOINCREMENT,
--                         userId INTEGER,
--                         friendId INTEGER,
--                         FOREIGN KEY (userId) REFERENCES user (id),
--                         FOREIGN KEY (friendId) REFERENCES user (id)
-- );
--
-- CREATE TABLE friendRequest (
--                                id INTEGER PRIMARY KEY AUTOINCREMENT,
--                                userId INTEGER,
--                                timestamp TIMESTAMP default(datetime('now','localtime')),
--                                text TEXT,
--                                requestUserId INTEGER,
--                                status INTEGER default(0),
--                                FOREIGN KEY (userId) REFERENCES user (id),
--                                FOREIGN KEY (requestUserId) REFERENCES user (id)
-- );
drop table groupChat;

CREATE TABLE IF NOT EXISTS user(
                      id INTEGER PRIMARY KEY,
                      username TEXT,
                      password TEXT,
                      nickname TEXT,
                      color TEXT,
                      avatar TEXT
);
CREATE TABLE IF NOT EXISTS groupChat (
                           gcid INTEGER PRIMARY KEY AUTOINCREMENT,
                           name TEXT,
                           owner INTEGER,
                           gctype TEXT,
                           avatar TEXT default(''),
                           color TEXT default(''),
                           msgNum INTEGER default(0),
                           last_msg_id INTEGER default(0),
                           last_msg_timestamp INTEGER default(1)
--                        timestamp is not zero for searching
);
CREATE TABLE IF NOT EXISTS groupUser(
                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                          gid INTEGER,
                          uid INTEGER
);

CREATE TABLE IF NOT EXISTS message (
                         id INTEGER PRIMARY KEY AUTOINCREMENT,
                         type TEXT,
                         content TEXT,
                         time INTEGER DEFAULT (strftime('%s', 'now', 'localtime')),
                         uid INTEGER,
                         mid INTEGER,
                         gid INTEGER
);
-- here need to have one pseduo message for join operation when the group has no message after create
insert or IGNORE into message (id,type,content,uid,mid,gid)values(0,'pseudo','testuse',0,0,0);

CREATE TRIGGER IF NOT EXISTS update_mid
    AFTER INSERT ON message
    FOR EACH ROW
    WHEN NEW.mid IS NULL
BEGIN
    UPDATE groupChat
    SET msgNum = msgNum + 1,
        last_msg_id = NEW.id,
        last_msg_timestamp = NEW.time
    WHERE groupChat.gcid = NEW.gid;

    UPDATE message
    SET mid = (
        SELECT groupChat.msgNum
        FROM groupChat
        WHERE groupChat.gcid = NEW.gid
    )
    WHERE id = NEW.id;
END;