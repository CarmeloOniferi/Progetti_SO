cmake_minimum_required(VERSION 3.28)
project(SO_server C)

set(CMAKE_C_STANDARD 11)
set(SOURCE_EXE src/main.c
        src/user.c include/user.h
        src/contact.c include/contact.h
        src/server.c include/server.h
        src/commands.c include/commands.h)
add_executable(SO_server ${SOURCE_EXE})