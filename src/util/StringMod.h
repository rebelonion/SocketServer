#pragma once
#include <string>


class StringMod {
public:
    static std::string color(const std::string &text, unsigned int color);
    static unsigned int chooseColor(unsigned int pos);
};