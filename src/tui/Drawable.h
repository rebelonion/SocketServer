#pragma once

struct Drawable {
    static constexpr wchar_t TOP_LEFT = L'┌';
    static constexpr wchar_t TOP_RIGHT = L'┐';
    static constexpr wchar_t BOTTOM_LEFT = L'└';
    static constexpr wchar_t BOTTOM_RIGHT = L'┘';
    static constexpr wchar_t HORIZONTAL = L'─';
    static constexpr wchar_t VERTICAL = L'│';

    static constexpr wchar_t LEFT_TEE = L'┤';
    static constexpr wchar_t RIGHT_TEE = L'├';
    static constexpr wchar_t BOTTOM_TEE = L'┴';
    static constexpr wchar_t TOP_TEE = L'┬';

    static constexpr wchar_t CROSS = L'┼';

    static constexpr wchar_t ESCAPE_SEQUENCE = L'\033';
};
