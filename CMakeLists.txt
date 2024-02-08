cmake_minimum_required(VERSION 3.20)

project(Subprocess LANGUAGES CXX)

file(GLOB_RECURSE SRCS "src/*.cpp")

add_executable(Subprocess ${SRCS})
