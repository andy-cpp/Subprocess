#include "Subprocess.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/wait.h>
#include <string.h>

void Subprocess::InitializeChildPipes()
{
    close(m_Pipes.Stdin[1]);
    dup2(m_Pipes.Stdin[0], STDIN_FILENO);

    close(m_Pipes.Stdout[0]);
    dup2(m_Pipes.Stdout[1], STDOUT_FILENO);

    close(m_Pipes.Stderr[0]);
    dup2((m_Flags & Flags::CombineOutput) ? m_Pipes.Stdout[1] : m_Pipes.Stderr[1], STDERR_FILENO);
}

void Subprocess::InitializeParentPipes()
{
    close(m_Pipes.Stdin[0]);
    close(m_Pipes.Stdout[1]);
    close(m_Pipes.Stderr[1]);
}

Subprocess::~Subprocess()
{
    /* Close remaining open pipes */
    close(m_Pipes.Stdin[1]);
    close(m_Pipes.Stdout[0]);
    close(m_Pipes.Stderr[0]);
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

std::unordered_map<std::string, std::string> GetEnv()
{
    std::unordered_map<std::string, std::string> env;
    char** envp = __environ;
    while(*envp)
    {
        std::string str(*envp);
        env[str.substr(0, str.find('='))] = str.substr(str.find('=')+1);
        envp++;
    }

    return env;
}

void Subprocess::Subprocess::Exec(char const* path, std::vector<std::string> const& argv, std::unordered_map<std::string, std::string> env)
{
    pipe(m_Pipes.Stdin);
    pipe(m_Pipes.Stdout);
    pipe(m_Pipes.Stderr);

    int pid = fork();
    if(pid == 0)
    {
        InitializeChildPipes();

        std::vector<char*> arguments;
        arguments.push_back(strdup(path));
        for(int i = 0; i < argv.size(); ++i)
        {
            arguments.push_back(strdup(argv[i].data()));
        }
        arguments.push_back(0);

        if(m_Flags & Flags::InheritEnv)
        {
            env.merge(GetEnv());
        }

        std::vector<char*> environment;
        for(auto& [key,value] : env)
        {
            environment.push_back(strdup(std::string(key + "=" + value).c_str()));
        }
        environment.push_back(0);

        execvpe(path, arguments.data(), environment.data());

        exit(0);
    }
    else {
        m_Pid = pid;
        InitializeParentPipes();
    }
}
