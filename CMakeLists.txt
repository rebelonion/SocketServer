cmake_minimum_required(VERSION 3.26)
project(SocketServer)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)

if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
elseif (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT")
elseif (UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif ()

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
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
        src/gui/Terminfo.h
        src/socket/interface/SocketInterface.h
        src/socket/interface/WindowsSocket.cpp
        src/socket/interface/WindowsSocket.h
        src/util/StringMod.cpp
        src/util/StringMod.h
)

if (WIN32)
    target_link_libraries(SocketServer ws2_32)
endif ()

if (UNIX AND NOT APPLE)
    target_link_libraries(SocketServer pthread)
endif ()