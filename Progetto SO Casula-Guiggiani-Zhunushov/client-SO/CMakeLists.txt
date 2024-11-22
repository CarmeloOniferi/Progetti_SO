cmake_minimum_required(VERSION 3.28)
project(SO_client C)

set(CMAKE_C_STANDARD 11)

add_executable(SO_client
        src/client.c
        src/commands.c
        include/commands.h
        include/client.h
)
