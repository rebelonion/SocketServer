
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
    std::wstring result;
    result.reserve(str.length()); // Reserve space for best-case scenario

    for (size_t i = 0; i < str.length(); ) {
        char32_t codepoint = 0;

        if (const auto c = static_cast<unsigned char>(str[i]); c <= 0x7F) {
            codepoint = c;
            i += 1;
        } else if (c <= 0xDF && i + 1 < str.length()) {
            codepoint = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
            i += 2;
        } else if (c <= 0xEF && i + 2 < str.length()) {
            codepoint = ((c & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
            i += 3;
        } else if (c <= 0xF7 && i + 3 < str.length()) {
            codepoint = ((c & 0x07) << 18) | ((str[i + 1] & 0x3F) << 12) | ((str[i + 2] & 0x3F) << 6) | (str[i + 3] & 0x3F);
            i += 4;
        } else {
            throw std::runtime_error("Invalid UTF-8 sequence");
        }

        if (codepoint <= 0xFFFF) {
            result.push_back(static_cast<wchar_t>(codepoint));
        } else if (codepoint <= 0x10FFFF) {
            // Encode as UTF-16 surrogate pair
            codepoint -= 0x10000;
            result.push_back(static_cast<wchar_t>(0xD800 | (codepoint >> 10)));
            result.push_back(static_cast<wchar_t>(0xDC00 | (codepoint & 0x3FF)));
        } else {
            throw std::runtime_error("Invalid Unicode code point");
        }
    }

    return result;
}

std::string StringMod::toString(const std::wstring &str) {
    std::string result;
    result.reserve(str.length() * 4); // Reserve space for worst-case scenario

    for (const char32_t c : str) {
        if (c <= 0x7F) {
            result.push_back(static_cast<char>(c));
        } else if (c <= 0x7FF) {
            result.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        } else if (c <= 0xFFFF) {
            result.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
            result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        } else if (c <= 0x10FFFF) {
            result.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
            result.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        } else {
            // Invalid Unicode code point
            throw std::runtime_error("Invalid Unicode code point");
        }
    }

    return result;
}