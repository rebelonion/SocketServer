cmake_minimum_required(VERSION 3.26)
project(SocketServer)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)

if (MINGW)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
    endif ()
elseif (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT")
elseif (UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif ()

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tui
        ${CMAKE_CURRENT_SOURCE_DIR}/src/socket
)

add_executable(SocketServer
        src/main.cpp
        src/socket/Socket.cpp
        src/socket/Socket.h
        src/util/Logger.cpp
        src/util/Logger.h
        src/Globals.h
        src/socket/ServerSocket.cpp
        src/socket/ServerSocket.h
        src/socket/ClientSocket.cpp
        src/socket/ClientSocket.h
        src/Globals.cpp
        src/socket/MessageQueue.cpp
        src/socket/MessageQueue.h
        src/tui/Terminfo.h
        src/socket/interface/SocketInterface.h
        src/socket/interface/WindowsSocket.cpp
        src/socket/interface/WindowsSocket.h
        src/util/StringMod.cpp
        src/util/StringMod.h
        src/socket/interface/LinuxSocket.cpp
        src/socket/interface/LinuxSocket.h
        src/tui/Buffer.cpp
        src/tui/Buffer.h
        src/tui/Drawable.h
        src/tui/Movable.cpp
        src/tui/Movable.h
        src/tui/TUIItem.h
        src/tui/TextBox.cpp
        src/tui/TextBox.h
        src/util/ThreadSafeVector.cpp
        src/util/ThreadSafeVector.h
        src/tui/TUI.cpp
        src/tui/TUI.h
        src/util/UserInput.cpp
        src/util/UserInput.h
        src/tui/Box.cpp
        src/tui/Box.h
        src/tui/Spinners.cpp
        src/tui/Spinners.h
        src/util/Errors.h
)

if (WIN32)
    target_link_libraries(SocketServer ws2_32)
endif ()

if (UNIX AND NOT APPLE)
    target_link_libraries(SocketServer pthread)
endif ()