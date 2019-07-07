cmake_minimum_required(VERSION 3.14)

project(c_errs C)

set(CMAKE_C_STANDARD 11)

add_executable(
        c_errs_error_test
        c_errs_error.h
        c_errs_error_test.c
)
