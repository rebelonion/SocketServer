#include "StringMod.h"

#include <locale>
#include <format>

#include "Drawable.h"
#include "Errors.h"

std::wstring StringMod::color(const std::wstring &text, const unsigned int color) {
    return std::format(L"\o{33}[{}m{}\o{33}[0m", color, text);
}

std::string StringMod::color(const std::string &text, const unsigned int color) {
    return std::format("\o{33}[{}m{}\o{33}[0m", color, text);
}

unsigned int StringMod::chooseColor(const size_t pos) {
    return 32 + (pos % 5);
}

std::wstring StringMod::toWString(const std::string &str) {
    std::wstring result;
    result.reserve(str.length()); // Reserve space for best-case scenario

    for (size_t i = 0; i < str.length();) {
        char32_t codepoint = 0;

        if (const auto c = static_cast<unsigned char>(str[i]); c <= 0x7F) {
            codepoint = c;
            i += 1;
        } else if (c <= 0xDF && i + 1 < str.length()) {
            codepoint = ((static_cast<uint32_t>(c) & 0x1F) << 6) |
                     (static_cast<uint32_t>(static_cast<unsigned char>(str[i + 1])) & 0x3F);
            i += 2;
        } else if (c <= 0xEF && i + 2 < str.length()) {
            codepoint = ((static_cast<uint32_t>(c) & 0x0F) << 12) |
                     ((static_cast<uint32_t>(static_cast<unsigned char>(str[i + 1])) & 0x3F) << 6) |
                     (static_cast<uint32_t>(static_cast<unsigned char>(str[i + 2])) & 0x3F);
            i += 3;
        } else if (c <= 0xF7 && i + 3 < str.length()) {
            codepoint = ((static_cast<uint32_t>(c) & 0x07) << 18) |
                     ((static_cast<uint32_t>(static_cast<unsigned char>(str[i + 1])) & 0x3F) << 12) |
                     ((static_cast<uint32_t>(static_cast<unsigned char>(str[i + 2])) & 0x3F) << 6) |
                     (static_cast<uint32_t>(static_cast<unsigned char>(str[i + 3])) & 0x3F);
            i += 4;
        } else {
            throw ParseError("Invalid UTF-8 sequence");
        }

        if (codepoint <= 0xFFFF) {
            result.push_back(static_cast<wchar_t>(codepoint));
        } else if (codepoint <= 0x10FFFF) {
            // Encode as UTF-16 surrogate pair
            codepoint -= 0x10000;
            result.push_back(static_cast<wchar_t>(0xD800 | (codepoint >> 10)));
            result.push_back(static_cast<wchar_t>(0xDC00 | (codepoint & 0x3FF)));
        } else {
            throw ParseError("Invalid Unicode code point");
        }
    }

    return result;
}

std::string StringMod::toString(const std::wstring &str) {
    std::string result;
    result.reserve(str.length() * 4); // Reserve space for worst-case scenario

    for (const char32_t c: str) {
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
            throw ParseError("Invalid Unicode code point");
        }
    }

    return result;
}

int StringMod::viewableCharCount(const std::wstring &str) {
    int count = 0;
    bool in_escape_sequence = false;
    size_t i = 0;

    while (i < str.length()) {
        const wchar_t c = str[i];

        if (c == Drawable::ESCAPE_SEQUENCE) {
            in_escape_sequence = true;
            ++i;
            continue;
        }
        if (in_escape_sequence) {
            if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z')) {
                in_escape_sequence = false;
            }
            ++i;
            continue;
        }

        //other control characters and zero-width characters
        if (c < 32 || (c >= 0x200B && c <= 0x200F) || (c >= 0xFE00 && c <= 0xFE0F)) {
            ++i;
            continue;
        }

        //all other characters
        ++count;

        //surrogate pairs (emoji and other non-BMP characters)
        if (i + 1 < str.length() && (c >= 0xD800 && c <= 0xDBFF) && (str[i + 1] >= 0xDC00 && str[i + 1] <= 0xDFFF)) {
            i += 2; // move past both characters of the surrogate pair
        } else {
            ++i;
        }
    }
    return count;
}

bool StringMod::isViewable(const wchar_t c) {
    //control characters
    if (c < 32) {
        return false;
    }

    //emoji and other non-BMP characters
    if (isHighSurrogate(c) || isLowSurrogate(c)) {
        return true;
    }

    //zero-width characters
    if (c >= 0x200B && c <= 0x200F) {
        return false;
    }

    return true;
}

bool StringMod::isHighSurrogate(const wchar_t c) {
    return c >= 0xD800 && c <= 0xDBFF;
}

bool StringMod::isLowSurrogate(const wchar_t c) {
    return c >= 0xDC00 && c <= 0xDFFF;
}

bool StringMod::isSurrogatePair(const wchar_t high, const wchar_t low) {
    return isHighSurrogate(high) && isLowSurrogate(low);
}

bool StringMod::isSingleUnicodeWideCharacter(const wchar_t c) {
    // East Asian Wide
    if ((c >= 0x1100 && c <= 0x115F) ||    // Hangul Jamo
        (c >= 0x2E80 && c <= 0x9FFF) ||    // CJK & Ideographs
        (c >= 0xAC00 && c <= 0xD7A3) ||    // Hangul Syllables
        (c >= 0xF900 && c <= 0xFAFF) ||    // CJK Compatibility Ideographs
        (c >= 0xFE10 && c <= 0xFE19) ||    // Vertical forms
        (c >= 0xFE30 && c <= 0xFE6F) ||    // CJK Compatibility Forms
        (c >= 0xFF00 && c <= 0xFF60) ||    // Fullwidth Forms
        (c >= 0xFFE0 && c <= 0xFFE6)) {    // Fullwidth Signs
        return true;
        }

    // Emoji & Symbols (single Unicode character)
    if ((c >= 0x2600 && c <= 0x27BF) ||    // Miscellaneous Symbols, Dingbats
        (c >= 0x2B50 && c <= 0x2B55) ||    // Miscellaneous Symbols and Arrows (includes ⭐)
        (c >= 0x2700 && c <= 0x27BF) ||    // Dingbats
        c == 0x303D ||                     // 〽 Part Alternation Mark
        c == 0x2733 ||                     // ✳ Eight Spoked Asterisk
        c == 0x2734 ||                     // ✴ Eight Pointed Black Star
        c == 0x2747) {                     // ❇ Sparkle
        return true;
        }

    return false;
}