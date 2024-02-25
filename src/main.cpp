#include <iostream>
#include <unistd.h>

#include <Subprocess/Subprocess.hpp>
int main()
{
    Subprocess process;
    process.Exec("env");

    process.Wait();
    std::cout << process.Read() << "\n";
    printf("Process exited with code %d\n", process.GetExitCode());
}
