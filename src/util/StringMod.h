#pragma once
#include <string>


class StringMod {
public:
    static std::wstring color(const std::wstring &text, unsigned int color);
    static std::string color(const std::string &text, unsigned int color);
    static unsigned int chooseColor(unsigned int pos);
    static std::wstring toWString(const std::string &str);
    static std::string toString(const std::wstring &str);
};