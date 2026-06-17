cmake_minimum_required(VERSION 3.20)

project(VJson LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Test Widgets)

function(vjson_enable_strict_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /WX
            /permissive-
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wshadow
            -Wold-style-cast
            -Woverloaded-virtual
            -Wnon-virtual-dtor
            -Werror
        )
    endif()
endfunction()

set(VJSON_SOURCES
    src/AppSettings.h
    src/AppSettings.cpp
    src/BuildInfo.h
    src/BuildInfo.cpp
    src/DemangleUtils.h
    src/DemangleUtils.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/Delegates.h
    src/Delegates.cpp
    src/LogFilterProxyModel.h
    src/LogFilterProxyModel.cpp
    src/CodeFormatter.h
    src/CodeFormatter.cpp
    src/FileUtils.h
    src/FileUtils.cpp
    src/FilterUtils.h
    src/FilterUtils.cpp
    src/HtmlUtils.h
    src/HtmlUtils.cpp
    src/IconUtils.h
    src/IconUtils.cpp
    src/JsonSyntaxHighlighter.h
    src/JsonSyntaxHighlighter.cpp
    src/LogLevelStyle.h
    src/LogLevelStyle.cpp
    src/JsonlModel.h
    src/JsonlModel.cpp
    src/JsonlRecord.h
    src/JsonUtils.h
    src/JsonUtils.cpp
    src/ThemeManager.h
    src/ThemeManager.cpp
    src/TimestampUtils.h
    src/TimestampUtils.cpp
)

add_library(VJsonLib STATIC
    ${VJSON_SOURCES}
)

target_compile_definitions(VJsonLib PRIVATE VJSON_BUILD_TYPE="$<CONFIG>")
target_include_directories(VJsonLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(VJsonLib PUBLIC Qt6::Widgets)
vjson_enable_strict_warnings(VJsonLib)

add_executable(VJson
    src/main.cpp
)

target_link_libraries(VJson PRIVATE VJsonLib)
vjson_enable_strict_warnings(VJson)

enable_testing()
add_subdirectory(tests)
