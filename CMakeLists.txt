cmake_minimum_required(VERSION 3.26)
project(reactor)

set(CMAKE_CXX_STANDARD 14)

add_executable(reactor main.cpp
        include/reactor/channel.h
        src/reactor/channel.c
        include/reactor/channel_map.h
        src/reactor/channel_map.c
        include/reactor/dispatcher.h
        include/reactor/event_loop.h
        src/reactor/event_loop.c
        src/reactor/epoll_dispatcher.c
)
