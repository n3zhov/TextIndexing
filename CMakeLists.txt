cmake_minimum_required(VERSION 3.21)
project(RedBeltFinal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

include_directories(.)

add_executable(RedBeltFinal
        iterator_range.h
        main.cpp
        parse.cpp
        parse.h
        search_server.cpp
        search_server.h
        test_runner.h
        profile.h
        synchronized.h)

target_link_libraries(RedBeltFinal Threads::Threads)