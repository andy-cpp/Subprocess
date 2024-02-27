# Subprocess

Subprocess is a c++ library that simplifies creating and running child processes on POSIX systems.

# Features
- [x] Ease of use
- [x] Pipes input / output
- [x] Option to seperate stdout and stderr
- [x] Supports environment variables


# Usage
To use Subprocess in a project clone this repository and add it to your CMakeLists.txt

Here is an example of how you can use Subprocess in cmake.
```cmake
cmake_minimum_required(VERSION 3.10)

project(Example)
# Include Subprocess
add_subdirectory(Subprocess)

add_executable(Example main.cpp)
target_link_libraries(Example Subprocess)
```

# Code example

```c++
#include <iostream>
#include <Subprocess/Subprocess.hpp>

int main()
{
    Subprocess process;
    process.Exec("pwd");

    std::cout << process.Read() << "\n";

    /* Execute a command with arguments */
    process.Exec("sleep", {"1"});
    process.Wait();

    /*
        Execute a command with env vars
        By default environment variables are inherited
        by the parent process, any variables passed to
        Exec will overwrite the inherited variables.
    */
    process.Exec("env", {}, {{"MyVariable", "100"}});
    std::cout << process.Read() << "\n";
}
```
