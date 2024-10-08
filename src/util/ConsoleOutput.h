#pragma once

#include <iostream>
#include <string>

class ConsoleOutput {
public:
    ConsoleOutput();

    ConsoleOutput& operator<<(const std::wstring& value);

    ConsoleOutput& operator<<(std::ostream& (*manipulator)(std::ostream&));

    static void flush();
};