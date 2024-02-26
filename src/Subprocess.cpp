#include <Subprocess/Subprocess.hpp>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/wait.h>
#include <string.h>

void Subprocess::Pipes::Print() const
{
    printf("Stdin  read (%d) write (%d)\n", Stdin[0], Stdin[1]);
    printf("Stdout read (%d) write (%d)\n", Stdout[0], Stdout[1]);
    printf("Stderr read (%d) write (%d)\n", Stderr[0], Stderr[1]);
}

static void CloseFD(int& fd)
{
    if(fd > 0){
        close(fd);
        fd = 0;
    }
}

void Subprocess::Pipes::Close()
{
    CloseFD(Stdin[0]);
    CloseFD(Stdin[1]);

    CloseFD(Stdout[0]);
    CloseFD(Stdout[1]);

    CloseFD(Stderr[0]);
    CloseFD(Stderr[1]);
}

void Subprocess::InitializeChildPipes()
{
    CloseFD(m_Pipes.Stdin[1]);
    dup2(m_Pipes.Stdin[0], STDIN_FILENO);

    CloseFD(m_Pipes.Stdout[0]);
    dup2(m_Pipes.Stdout[1], STDOUT_FILENO);

    CloseFD(m_Pipes.Stderr[0]);
    dup2((m_Flags & Flags::CombineOutput) ? m_Pipes.Stdout[1] : m_Pipes.Stderr[1], STDERR_FILENO);
}

void Subprocess::InitializeParentPipes()
{
    /*
        Close child end of pipes
    */
    CloseFD(m_Pipes.Stdin[0]);
    CloseFD(m_Pipes.Stdout[1]);
    CloseFD(m_Pipes.Stderr[1]);
}

Subprocess::~Subprocess()
{
    /* Close remaining open pipes */
    CloseFD(m_Pipes.Stdin[1]);
    CloseFD(m_Pipes.Stdout[0]);
    CloseFD(m_Pipes.Stderr[0]);
}

bool HasData(int fd, size_t usec)
{
    fd_set fset;
    FD_ZERO(&fset);
    FD_SET(fd, &fset);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = usec;
    int ret = select(fd + 1, &fset, 0,0, &timeout);
    return ret > 0;
}

static std::string ReadFromFD(int fd, size_t timeout = 0)
{
    if(timeout > 0 && !HasData(fd, timeout * 1000))
    {
        return std::string(); // Timed out
    }
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

std::string Subprocess::Read(ms_t timeoutms)
{
    return ReadFromFD(m_Pipes.Stdout[0], timeoutms);
}

std::string Subprocess::ReadStderr(ms_t timeoutms)
{
    return ReadFromFD(m_Pipes.Stderr[0], timeoutms);
}

int Subprocess::GetExitCode() const
{
    int s = 0;
    pid_t pid = waitpid(m_Pid, &s, WNOHANG);
    return WEXITSTATUS(s);
}

bool Subprocess::IsRunning() const
{
    int s = 0;
    pid_t pid = waitpid(m_Pid, &s, WNOHANG);
    return pid == 0;
}

void Subprocess::Wait()
{
    int s = 0;
    waitpid(m_Pid, &s, 0);
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

bool Subprocess::Subprocess::Exec(char const* path, std::vector<std::string> const& argv, std::unordered_map<std::string, std::string> env)
{
    /*
        Close any possibly existing pipes that might already exist.
    */
    m_Pipes.Close();

    m_Pid = 0;

    pipe(m_Pipes.Stdin);
    pipe(m_Pipes.Stdout);
    pipe(m_Pipes.Stderr);

    int pid = fork();
    if(pid == 0)
    {
        InitializeChildPipes();
        closefrom(3);

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

        int status = execvpe(path, arguments.data(), environment.data());
        printf("execvpe error %s\n", strerror(errno));

        exit(errno);
    }
    else if(pid > 0) {
        m_Pid = pid;
        InitializeParentPipes();
    }
    return pid > 0;
}
