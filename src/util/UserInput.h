#pragma once

#include <optional>
#include <string>
#ifdef __linux__
#include <termios.h>
#endif

class UserInput {
public:
    bool inputAvailable();

    std::optional<std::wstring> getInput(std::wstring oldVal);

    static UserInput& getInstance();

    UserInput(const UserInput&) = delete;
    UserInput& operator=(const UserInput&) = delete;

private:
#ifdef __linux__
    termios orig_termios{};
#endif
    bool is_raw_mode;

    UserInput() : is_raw_mode(false) {}

#ifndef _WIN32
    char getch();

    void enableRawMode();
#endif
};
