#include <iostream>
#include <unistd.h>

#include "Subprocess.hpp"
int main()
{
    Subprocess process;
    process.Exec("env");
    Subprocess::Pipes const& pipes = process.GetPipes();

    char buffer[32768] = {0};
    read(pipes.Stdout[0], buffer, sizeof buffer);
    printf("%s\n", buffer);
}
