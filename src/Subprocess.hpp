#pragma once
#include <string>
#include <vector>
#include <unordered_map>
class Subprocess
{
public:
    Subprocess() = default;

    struct Pipes{
        int Stdin[2];
        int Stdout[2];
        int Stderr[2];
    };
    void Write(std::string const& data);

    std::string Read();

    std::string ReadStderr();

    Pipes const& GetPipes() const { return m_Pipes; }

    enum Flags{
        // Combines stdout and stderr, useful to read all information in chronological order.
        CombineOutput = 1 << 0,
        // Inherits parent process env, other env variables will overwrite parent env variables
        InheritEnv = 1 << 1,
    };

    void Exec(char const* path, std::vector<std::string> const& argv = {}, std::unordered_map<std::string, std::string> = {});

private:
    void InitializeChildPipes();

    void InitializeParentPipes();

private:
    pid_t m_Pid = 0;
    Pipes m_Pipes = {};
    int m_Flags = Flags::CombineOutput | Flags::InheritEnv;
};
