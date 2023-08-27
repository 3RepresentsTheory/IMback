CREATE TABLE user (
                            id INTEGER PRIMARY KEY,
                            username TEXT,
                            password TEXT,
                            nickname TEXT,
                            color TEXT,
                            avatar TEXT
);
CREATE TABLE friend (
                        id INTEGER PRIMARY KEY AUTOINCREMENT,
                        userId INTEGER,
                        friendId INTEGER,
                        FOREIGN KEY (userId) REFERENCES user (id),
                        FOREIGN KEY (friendId) REFERENCES user (id)
);

CREATE TABLE friendRequest (
                               id INTEGER PRIMARY KEY AUTOINCREMENT,
                               userId INTEGER,
                               timestamp TIMESTAMP default(datetime('now','localtime')),
                               text TEXT,
                               requestUserId INTEGER,
                               status INTEGER default(0),
                               FOREIGN KEY (userId) REFERENCES user (id),
                               FOREIGN KEY (requestUserId) REFERENCES user (id)
);
CREATE TABLE groupChat (
                           id INTEGER PRIMARY KEY AUTOINCREMENT,
                           name TEXT,
                           msgNum INTEGER default(0),
                           owner INTEGER,
                           type TEXT,
                           last_msg_id INTEGER,
                           last_msg_timestamp INTEGER
);

CREATE TABLE groupUser(
                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                          gid INTEGER,
                          uid INTEGER,
                          FOREIGN KEY (gid) REFERENCES groupChat (id),
                          FOREIGN KEY (uid) REFERENCES user (id)
);

DROP TABLE message;
CREATE TABLE message (
                         id INTEGER PRIMARY KEY AUTOINCREMENT,
                         type TEXT,
                         content TEXT,
                         time INTEGER DEFAULT (strftime('%s', 'now', 'localtime')),
                         uid INTEGER,
                         mid INTEGER,
                         gid INTEGER,
                         FOREIGN KEY (uid) REFERENCES user (id),
                         FOREIGN KEY (gid) REFERENCES groupChat (id)
);


drop trigger update_mid;
CREATE TRIGGER update_mid
    AFTER INSERT ON message
    FOR EACH ROW
    WHEN NEW.mid IS NULL
BEGIN
    UPDATE groupChat
    SET msgNum = msgNum + 1,
        last_msg_id = NEW.id,
        last_msg_timestamp = NEW.time
    WHERE groupChat.id = NEW.gid;

    UPDATE message
    SET mid = (
        SELECT msgNum
        FROM groupChat
        WHERE gid = NEW.gid
    )
    WHERE id = NEW.id;
END;