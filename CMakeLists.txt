cmake_minimum_required(VERSION 3.10)
project(fconvert)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES "main.cpp" "classes/*.cpp")

add_executable(fconvert ${SOURCES})

target_link_libraries(fconvert ${OpenCV_LIBS})

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_link_libraries(fconvert stdc++fs)
endif()