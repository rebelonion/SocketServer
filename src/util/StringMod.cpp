
#include "StringMod.h"

#include <locale>
#include <codecvt>

std::wstring StringMod::color(const std::wstring &text, const unsigned int color) {
    return L"\033[" + std::to_wstring(color) + L"m" + text + L"\033[0m";
}

std::string StringMod::color(const std::string &text, const unsigned int color) {
    return "\033[" + std::to_string(color) + "m" + text + "\033[0m";
}

unsigned int StringMod::chooseColor(const unsigned int pos) {
    return 32 + (pos % 5);
}

std::wstring StringMod::toWString(const std::string &str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX> converterX;

    return converterX.from_bytes(str);
}

std::string StringMod::toString(const std::wstring &str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX> converterX;

    return converterX.to_bytes(str);
}