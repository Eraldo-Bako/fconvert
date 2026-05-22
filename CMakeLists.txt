cmake_minimum_required(VERSION 3.12)
project(fconvert LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

file(GLOB CLASS_SOURCES CONFIGURE_DEPENDS 
    "classes/*.cpp"
    "classes/*.h"
)

add_executable(fconvert main.cpp ${CLASS_SOURCES})

target_include_directories(fconvert PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(fconvert PRIVATE ${OpenCV_LIBS})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(fconvert PRIVATE stdc++fs)
endif()