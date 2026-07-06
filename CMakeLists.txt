cmake_minimum_required(VERSION 3.16)
project(BankVision VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/mingw_64")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/models
    ${CMAKE_SOURCE_DIR}/views
    ${CMAKE_SOURCE_DIR}/views/widgets
    ${CMAKE_SOURCE_DIR}/controllers
    ${CMAKE_SOURCE_DIR}/data
)

file(GLOB SOURCES_ROOT "${CMAKE_SOURCE_DIR}/main.cpp")
file(GLOB_RECURSE SOURCES_MODELS   "${CMAKE_SOURCE_DIR}/models/*.cpp")
file(GLOB_RECURSE SOURCES_VIEWS    "${CMAKE_SOURCE_DIR}/views/*.cpp")
file(GLOB_RECURSE SOURCES_CTRL     "${CMAKE_SOURCE_DIR}/controllers/*.cpp")
file(GLOB_RECURSE SOURCES_DATA     "${CMAKE_SOURCE_DIR}/data/*.cpp")

set(SOURCES
    ${SOURCES_ROOT}
    ${SOURCES_MODELS}
    ${SOURCES_VIEWS}
    ${SOURCES_CTRL}
    ${SOURCES_DATA}
)

set(RESOURCES "${CMAKE_SOURCE_DIR}/resources/resources.qrc")

add_executable(BankVision ${SOURCES} ${RESOURCES})

target_link_libraries(BankVision PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core Qt6::Sql)
