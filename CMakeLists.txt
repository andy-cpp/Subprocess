cmake_minimum_required(VERSION 3.20)

project(Subprocess LANGUAGES CXX)

set(LIBSRC "src/Subprocess.cpp")
add_library(Subprocess STATIC ${LIBSRC})
target_include_directories(Subprocess PUBLIC "include/")

add_executable(SubprocessExample "src/main.cpp")
target_link_libraries(SubprocessExample Subprocess)
set_target_properties(SubprocessExample PROPERTIES OUTPUT_NAME "Subprocess")
