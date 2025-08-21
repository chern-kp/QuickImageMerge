cmake_minimum_required(VERSION 3.16)
project(QuickImageMerge)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.9.1/mingw_64")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(QuickImageMerge
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    imageprocessor.cpp
    imageprocessor.h
)

target_link_libraries(QuickImageMerge PRIVATE Qt6::Widgets)