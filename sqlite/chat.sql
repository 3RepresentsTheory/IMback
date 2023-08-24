CREATE TABLE userDetail (
                            id INTEGER PRIMARY KEY,
                            username TEXT,
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
                               timestamp TEXT,
                               requestText TEXT,
                               requestUserId INTEGER,
                               FOREIGN KEY (userId) REFERENCES user (id),
                               FOREIGN KEY (requestUserId) REFERENCES user (id)
);
CREATE TABLE groupChat (
                           id INTEGER PRIMARY KEY AUTOINCREMENT,
                           nickname TEXT
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
                         time INTEGER,
                         uid INTEGER,
                         mid INTEGER,
                         gid INTEGER,
                         FOREIGN KEY (uid) REFERENCES user (id),
                         FOREIGN KEY (gid) REFERENCES groupChat (id)
);

CREATE TABLE messageCounter (
                                gid INTEGER,
                                counter INTEGER DEFAULT 0,
                                PRIMARY KEY (gid),
                                FOREIGN KEY (gid) REFERENCES groupChat (id)
);

CREATE TRIGGER update_mid
    AFTER INSERT ON message
    FOR EACH ROW
    WHEN NEW.mid IS NULL
BEGIN
    UPDATE messageCounter
    SET counter = counter + 1
    WHERE gid = NEW.gid;

    UPDATE message
    SET mid = (
        SELECT counter
        FROM messageCounter
        WHERE gid = NEW.gid
    )
    WHERE id = NEW.id;
END;