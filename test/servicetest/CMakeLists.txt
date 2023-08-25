cmake_minimum_required(VERSION 3.24)
project(service)

set(CMAKE_PREFIX_PATH "E:/software/Qt6.5.2/6.5.2/mingw_64")
find_package(Qt6 REQUIRED COMPONENTS WebSockets HttpServer)


qt_standard_project_setup()

qt_add_executable(messageServiceTest
        ../../sqlite/sqlite3.c
        ../../Dao/BaseDao.cpp
        ../../Dao/DataClasses.cpp
        ../../service/messageService.cpp
        messageServiceTest.cpp
)

target_link_libraries(messageServiceTest PRIVATE Qt6::HttpServer)

target_compile_options(messageServiceTest PRIVATE "-DTHREADSAFE=1")
