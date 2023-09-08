cmake_minimum_required(VERSION 3.24)


project(IMback)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network HttpServer Sql)

set(SOURCES
        dispatcher.cpp
        sqlite/sqlite3.c

        dao/BaseDao.cpp
        dao/DataClasses.cpp

        service/UserService.cpp
        service/FriendService.cpp
        service/MessageService.cpp
        service/GroupService.cpp

        apis/SessionApi.cpp
        apis/UserApi.cpp
        apis/FriendApi.cpp
        apis/MessageApi.cpp
        apis/GroupApi.cpp

        broadcast/chatserver.cpp
)

set(HEADERS
        utils/types.h
        utils/utils.h
        sqlite/sqlite3.h
        dao/BaseDao.h
        service/UserService.h
        service/MessageService.h
        service/GroupService.h
        broadcast/BroadCastThread.h
        broadcast/chatserver.h
        apis/GroupApi.h
        apis/FriendApi.h
        apis/MessageApi.h
        apis/SessionApi.h
        apis/UserApi.h
)

# 获取当前项目的绝对路径
get_filename_component(PROJECT_PATH "${CMAKE_SOURCE_DIR}" ABSOLUTE)
add_definitions(-DPROJECT_PATH="${PROJECT_PATH}")

add_executable(IMback ${SOURCES} ${HEADERS})

target_link_libraries(IMback
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::HttpServer
        Qt::Sql
)
