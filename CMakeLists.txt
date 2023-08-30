cmake_minimum_required(VERSION 3.24)


project(Demo02)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH "/home/flwfdd/program/Qt/6.5.2/gcc_64")


find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network HttpServer Sql)

set(SOURCES
        dispatcher.cpp
        sqlite/sqlite3.c
        dao/BaseDao.cpp
        service/UserService.cpp
        service/FriendService.cpp
        service/MessageService.cpp
        apis/SessionApi.cpp
        apis/UserApi.cpp
        apis/FriendApi.cpp
        apis/MessageApi.cpp
        broadcast/chatserver.cpp
        dao/DataClasses.cpp apis/GroupApi.cpp service/GroupService.cpp)
#        apis/MessageApi.cpp
#        service/MessageService.cpp
#        utils/Jsonable.cpp)

set(HEADERS
        utils/types.h
        utils/utils.h
        sqlite/sqlite3.h
        dao/BaseDao.h
        service/UserService.h
        service/MessageService.h
        service/GroupService.h
#        utils/Jsonable.h

        utils/utils.h
        broadcast/BroadCastThread.h
        broadcast/chatserver.h
        apis/GroupApi.h
        apis/FriendApi.h
        apis/MessageApi.h
        apis/SessionApi.h
        apis/UserApi.h

    )

add_executable(Demo02 ${SOURCES} ${HEADERS})

target_link_libraries(Demo02
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::HttpServer
        Qt::Sql
    )


#
#add_subdirectory(samplefiles/simpletest)
#add_subdirectory(test)
