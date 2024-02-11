#pragma once

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

    enum class Flags{
        // Combines stdout and stderr, useful to read all information in chronological order.
        COMBINE_OUTPUT = 1 << 0
    };

    void Exec(char const* path);

private:
    Pipes m_Pipes = {};
    Flags m_Flags = Flags::COMBINE_OUTPUT;
};
