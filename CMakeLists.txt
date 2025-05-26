cmake_minimum_required(VERSION 3.30)
project(mini_network_proxy C)

set(CMAKE_C_STANDARD 11)

add_executable(mini_network_proxy src/main.c
        src/proxy.c
        include/proxy.h)
