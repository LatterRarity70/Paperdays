cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(main)

file(GLOB SOURCES src/*.cpp)
message(${SOURCES})
add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

file(DOWNLOAD "https://github.com/user95401/geode-download-dependency-cmake/blob/main/.cmake?raw=true" 
    ${CMAKE_CURRENT_BINARY_DIR}/geode-download-dependency.cmake)
include(${CMAKE_CURRENT_BINARY_DIR}/geode-download-dependency.cmake)

download_dependency_mod(user95401/geode-game-objects-factory user95401.game-objects-factory)

setup_geode_mod(${PROJECT_NAME} EXTERNALS user95401.game-objects-factory)