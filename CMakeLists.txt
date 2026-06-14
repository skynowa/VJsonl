cmake_minimum_required(VERSION 3.20)

project(VJson LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(VJson
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/LogFilterProxyModel.h
    src/LogFilterProxyModel.cpp
    src/CodeFormatter.h
    src/CodeFormatter.cpp
    src/FileUtils.h
    src/FileUtils.cpp
    src/HtmlUtils.h
    src/HtmlUtils.cpp
    src/LogLevelStyle.h
    src/LogLevelStyle.cpp
    src/JsonlModel.h
    src/JsonlModel.cpp
    src/JsonlRecord.h
    src/JsonUtils.h
    src/JsonUtils.cpp
)

target_link_libraries(VJson PRIVATE Qt6::Widgets)
