cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Basalt VERSION 0.3)

add_subdirectory("external")
add_subdirectory("runtime")

add_subdirectory("sandbox")

add_subdirectory("app")
