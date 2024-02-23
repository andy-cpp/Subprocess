#include <iostream>
#include <unistd.h>

#include "Subprocess.hpp"
int main()
{
    Subprocess process;
    process.Exec("env");

    std::cout << process.Read() << "\n";
}
