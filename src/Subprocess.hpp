#pragma once
#include <string>
#include <vector>
#include <unordered_map>
class Subprocess
{
public:
    Subprocess() = default;

    ~Subprocess();

    /*
        Type used for timeouts, values should be in milliseconds.
    */
    typedef size_t ms_t;
    struct Pipes{
        int Stdin[2];
        int Stdout[2];
        int Stderr[2];

        void Print() const;
    };
    void Write(std::string const& data);

    std::string Read(ms_t timeout = 0);

    std::string ReadStderr(ms_t timeout = 0);

    Pipes const& GetPipes() const { return m_Pipes; }

    enum Flags{
        // Combines stdout and stderr, useful to read all information in chronological order.
        CombineOutput = 1 << 0,
        // Inherits parent process env, other env variables will overwrite parent env variables
        InheritEnv = 1 << 1,
    };

    size_t GetFlags() const { return m_Flags; }

    void SetFlags(size_t flags) { m_Flags = flags; }

    void Exec(char const* path, std::vector<std::string> const& argv = {}, std::unordered_map<std::string, std::string> = {});

private:
    void InitializeChildPipes();

    void InitializeParentPipes();

private:
    pid_t m_Pid = 0;
    Pipes m_Pipes = {};
    size_t m_Flags = Flags::CombineOutput | Flags::InheritEnv;
};
