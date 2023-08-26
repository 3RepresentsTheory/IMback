cmake_minimum_required(VERSION 3.24)

set(CMAKE_PREFIX_PATH "D:/qt/6.5.2/mingw_64")

project(Demo02)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Network HttpServer REQUIRED)

set(SOURCES
        dispatcher.cpp
        sqlite/sqlite3.c
        service/UserService.cpp
        dao/BaseDao.cpp
        apis/SessionApi.cpp
        apis/FriendApi.cpp
        service/FriendService.cpp
        apis/UserApi.cpp
        apis/MessageApi.cpp broadcast/chatserver.cpp)
#        apis/MessageApi.cpp
#        service/MessageService.cpp
#        utils/Jsonable.cpp)

set(HEADERS
        utils/types.h
        utils/utils.h
        sqlite/sqlite3.h
        dao/BaseDao.h
        service/UserService.h
#        utils/Jsonable.h
#        apis/SessionApi.h
#        apis/MessageApi.h
        utils/utils.h
        broadcast/BroadCastThread.h
        broadcast/chatserver.h
        service/MessageService.h)

add_executable(Demo02 ${SOURCES} ${HEADERS})

target_link_libraries(Demo02 Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::HttpServer)
#
#add_subdirectory(samplefiles/simpletest)
#add_subdirectory(test)
