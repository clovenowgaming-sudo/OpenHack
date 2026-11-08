cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(OpenHack27 VERSION 2.7.0 HOMEPAGE_URL "https://github.com/vinz/OpenHack-2.7")

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Dear ImGui + imgui-cocos (touch input supported on Android)
CPMAddPackage("gh:Prevter/gd-imgui-cocos#6383c75f7c3ec1ef63d24e0bc757dd5d4d326f18")

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/third_party
    ${CMAKE_CURRENT_LIST_DIR}/third_party/imgui_markdown
)

target_compile_definitions(${PROJECT_NAME} PRIVATE OPENHACK_GEODE=1)

target_link_libraries(${PROJECT_NAME} imgui-cocos)

setup_geode_mod(${PROJECT_NAME})
