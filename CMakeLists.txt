cmake_minimum_required(VERSION 3.12)
project(fconvert LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WITH_LIBRAW "Compile with LibRaw support for RAW image formats" ON)

find_package(OpenCV REQUIRED)

if(WITH_LIBRAW)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(LIBRAW QUIET libraw)
    endif()

    if(NOT LIBRAW_FOUND)
        find_package(LibRaw QUIET) 
        if(LibRaw_FOUND)
            set(LIBRAW_FOUND TRUE)
            set(LIBRAW_INCLUDE_DIRS ${LibRaw_INCLUDE_DIR})
            set(LIBRAW_LIBRARIES ${LibRaw_LIBRARIES})
        endif()
    endif()

    if(NOT LIBRAW_FOUND)
        message(WARNING "LibRaw not found. Building without RAW image support.")
    endif()
endif()

file(GLOB CLASS_SOURCES CONFIGURE_DEPENDS 
    "classes/*.cpp"
    "classes/*.h"
)

if(WIN32)
    list(APPEND WINDOWS_RESOURCES resources.rc)
endif()

add_executable(fconvert main.cpp ${CLASS_SOURCES} ${WINDOWS_RESOURCES})

target_link_libraries(fconvert PRIVATE ${OpenCV_LIBS}) 

if(WITH_LIBRAW AND LIBRAW_FOUND)
    target_include_directories(fconvert PRIVATE ${LIBRAW_INCLUDE_DIRS})
    target_link_libraries(fconvert PRIVATE ${LIBRAW_LIBRARIES})
    target_compile_definitions(fconvert PRIVATE HAS_LIBRAW)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(fconvert PRIVATE stdc++fs)
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