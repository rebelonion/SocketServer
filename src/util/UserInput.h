#pragma once

#include <optional>
#include <string>

class UserInput {
public:
    static bool inputAvailable();


    static std::optional<std::wstring> getInput(std::wstring oldVal);

private:
    static char getch();
};
