cmake_minimum_required(VERSION 3.16)
project(Bookstore)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-O2)

set(src_dir
  src/main.cpp
)

add_executable(code ${src_dir})