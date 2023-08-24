cmake_minimum_required(VERSION 3.16)
project(testqt LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "E:/software/Qt6.5.2/6.5.2/mingw_64")
find_package(Qt6 REQUIRED COMPONENTS WebSockets HttpServer)


qt_standard_project_setup()

qt_add_executable(testqt
        test.cpp
)
