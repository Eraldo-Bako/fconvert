# fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
# Licensed under the Apache License, Version 2.0 (the "License")
# Maintainer: eraldobako@gmail.com

cmake_minimum_required(VERSION 3.15)
project(fconvert LANGUAGES CXX)

if(WIN32)
    add_compile_definitions(NOMINMAX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WITH_LIBRAW "Compile with LibRaw support for RAW image formats" ON)

find_package(OpenCV REQUIRED)

if(WIN32 AND VCPKG_TARGET_TRIPLET)
    link_directories("${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib")
endif()

if(WITH_LIBRAW)

    find_package(libraw CONFIG QUIET)

    if(libraw_FOUND)
        set(LIBRAW_FOUND TRUE)
        set(LIBRAW_LIBRARIES libraw::raw)
    else()
        find_package(PkgConfig QUIET)
        if(PKG_CONFIG_FOUND)
            pkg_check_modules(LIBRAW QUIET libraw)
        endif()

        if(NOT LIBRAW_FOUND)
            find_package(LibRaw CONFIG QUIET)
            if(NOT LibRaw_FOUND)
                find_package(LibRaw QUIET)
            endif()

            if(TARGET LibRaw::LibRaw)
                set(LIBRAW_FOUND TRUE)
                set(LIBRAW_LIBRARIES LibRaw::LibRaw)
            elseif(LibRaw_FOUND)
                set(LIBRAW_FOUND TRUE)
                set(LIBRAW_INCLUDE_DIRS ${LibRaw_INCLUDE_DIR})
                set(LIBRAW_LIBRARIES ${LibRaw_LIBRARIES})
            endif()
        endif()
    endif()

    if(NOT LIBRAW_FOUND)
        message(FATAL_ERROR "LibRaw requested but not found!")
    endif()
    
endif()

file(GLOB CLASS_SOURCES CONFIGURE_DEPENDS 
    "classes/*.cpp"
    "classes/*.hpp"
    "classes/program/*.cpp"
    "classes/program/*.hpp"
)

if(WIN32)
    list(APPEND WINDOWS_RESOURCES resources.rc)
endif()

add_executable(fconvert main.cpp ${CLASS_SOURCES} ${WINDOWS_RESOURCES})

target_link_libraries(fconvert PRIVATE ${OpenCV_LIBS}) 

if(WITH_LIBRAW AND LIBRAW_FOUND)

    if(TARGET "${LIBRAW_LIBRARIES}")
        target_link_libraries(fconvert PRIVATE "${LIBRAW_LIBRARIES}")
    else()
        target_include_directories(fconvert PRIVATE ${LIBRAW_INCLUDE_DIRS})
        if(LIBRAW_LIBRARY_DIRS)
            target_link_directories(fconvert PRIVATE ${LIBRAW_LIBRARY_DIRS})
        endif()
        
        target_link_libraries(fconvert PRIVATE ${LIBRAW_LIBRARIES})
    endif()
    
    target_compile_definitions(fconvert PRIVATE HAS_LIBRAW)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(fconvert PRIVATE stdc++fs)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(fconvert PRIVATE $<$<CONFIG:Release>:-O3>)
    target_link_options(fconvert PRIVATE $<$<CONFIG:Release>:-s>)
endif()

# APPIMAGE
if(NOT WIN32)
    include(GNUInstallDirs)

    install(TARGETS fconvert DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES fconvert.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES branding/icon.svg 
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
            RENAME fconvert-icon.svg)
endif()
