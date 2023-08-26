# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(singleGroupTest LANGUAGES CXX)
set(CMAKE_PREFIX_PATH "E:/software/Qt6.5.2/6.5.2/mingw_64")

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

find_package(Qt6 REQUIRED COMPONENTS WebSockets HttpServer)

qt_standard_project_setup()

qt_add_executable(singleGroupTest
    chatserver.cpp
    chatserver.h
    main.cpp
        ../testClientAndReceiveApi/ClientListen.h ../testClientAndReceiveApi/ClientListen.cpp)

set_target_properties(singleGroupTest PROPERTIES
    WIN32_EXECUTABLE FALSE
    MACOSX_BUNDLE FALSE
)

target_link_libraries(singleGroupTest PUBLIC
    Qt::WebSockets
)
target_link_libraries(singleGroupTest PRIVATE
    Qt::HttpServer
)

qt_add_resources(singleGroupTest "assets"
        PREFIX
        "/"
        FILES
        assets/qt-logo.png
        )

