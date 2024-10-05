#pragma once
#include <string>


class StringMod {
public:
    static std::wstring color(const std::wstring &text, unsigned int color);

    static std::string color(const std::string &text, unsigned int color);

    static unsigned int chooseColor(size_t pos);

    static std::wstring toWString(const std::string &str);

    static std::string toString(const std::wstring &str);

    static int viewableCharCount(const std::wstring &str);

    static bool isViewable(wchar_t c);

    static bool isHighSurrogate(wchar_t c);

    static bool isLowSurrogate(wchar_t c);

    static bool isSurrogatePair(wchar_t high, wchar_t low);

    static bool isSingleUnicodeWideCharacter(wchar_t c);
};
