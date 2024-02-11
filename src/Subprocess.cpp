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

static std::string ReadFromFD(int fd)
{
    std::string data;
    while(true)
    {
        char buffer[1024] = {0};
        int nBytes = read(fd, buffer, sizeof(buffer));
        if(nBytes <= 0) break;
        data.append(buffer, nBytes);
        if(nBytes < sizeof(buffer)) break;
    }
    return data;
}

void Subprocess::Write(std::string const& data)
{
    write(m_Pipes.Stdin[1], data.c_str(), data.size());
}

std::string Subprocess::Read()
{
    return ReadFromFD(m_Pipes.Stdout[0]);
}

std::string Subprocess::ReadStderr()
{
    return ReadFromFD(m_Pipes.Stderr[0]);
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