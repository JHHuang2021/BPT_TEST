cmake_minimum_required(VERSION 3.16)
project(Bookstore)

set(CMAKE_CXX_STANDARD 20)
#add_compile_options(-std=c++2a -fconcepts)
add_compile_options(-g)

set(src_dir

        ${PROJECT_SOURCE_DIR}/BPlusTree/test.cpp
        )

add_executable(code ${src_dir})