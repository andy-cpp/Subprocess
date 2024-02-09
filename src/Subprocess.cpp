#include "Subprocess.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sys/wait.h>
#include <string.h>

void InitializeChildPipes(Subprocess::Pipes& pipes)
{
    close(pipes.Stdin[1]);
    dup2(pipes.Stdin[0], STDIN_FILENO);

    close(pipes.Stdout[0]);
    dup2(pipes.Stdout[1], STDOUT_FILENO);

    close(pipes.Stderr[0]);
    dup2(pipes.Stderr[1], STDERR_FILENO);
}

void InitializeParentPipes(Subprocess::Pipes& pipes)
{
    close(pipes.Stdin[0]);
    close(pipes.Stdout[1]);
    close(pipes.Stderr[1]);
}

void Subprocess::Subprocess::Exec(char const* path)
{
    pipe(m_Pipes.Stdin);
    pipe(m_Pipes.Stdout);
    pipe(m_Pipes.Stderr);

    int pid = fork();
    if(pid == 0)
    {
        InitializeChildPipes(m_Pipes);
        char* arguments[] = {strdup(path), 0};

        execvp(path, arguments);

        exit(0);
    }
    else {
        InitializeParentPipes(m_Pipes);
    }
}