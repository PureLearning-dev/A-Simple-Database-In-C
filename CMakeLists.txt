cmake_minimum_required(VERSION 4.2)
project(SimpleDataBase C)

set(CMAKE_C_STANDARD 99)

add_executable(SimpleDataBase main.c
        db.c
        db.h
        input.c
        statement.c)
