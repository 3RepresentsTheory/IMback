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
        Dao/userDao.cpp
        service/userService.cpp
        Dao/BaseDao.cpp
        HttpServerHeaders/sessionapi.cpp
        HttpServerHeaders/messageapi.cpp
        )

set(HEADERS
#        HttpServerHeaders/sessionapi.h
        HttpServerHeaders/types.h
        HttpServerHeaders/utils.h
        sqlite/sqlite3.h
        Dao/userDao.h
        Dao/BaseDao.h
        service/userService.h
        )

add_executable(Demo02 ${SOURCES} ${HEADERS})

target_link_libraries(Demo02 Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::HttpServer)

add_subdirectory(samplefiles/simpletest)
