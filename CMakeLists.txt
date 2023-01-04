cmake_minimum_required(VERSION 3.16)
project(Bookstore)

set(CMAKE_CXX_STANDARD 14)

add_executable(code main.cpp Blocks.h Command.h Account.h Log.h Books.h)
