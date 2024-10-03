#include "Buffer.h"

#include "Globals.h"
#include "StringMod.h"

Buffer::Buffer(const int width, const int height) {
    m_width = width;
    m_height = height;
    m_buffer.reserve(m_height);
    for (int y = 0; y < m_height; ++y) {
        m_buffer.emplace_back(m_width);
        for (int x = 0; x < m_width; ++x) {
            m_buffer[y][x] = L' ';
        }
    }
}

std::wstring Buffer::getLine(const int y) const {
    std::wstring result;
    if (y >= 0 && y < m_height) {
        for (int x = 0; x < m_width; ++x) {
            result += m_buffer[y][x];
        }
    }
    return result;
}

void Buffer::draw(const int x, const int y, const std::wstring_view str) {
    if (y < 0 || y >= m_height) return;

    int pos = x;
    size_t i = 0;
    bool in_escape_sequence = false;
    std::wstring current_cell;

    while (i < str.length() && pos < m_width) {
        if (str[i] == L'\033') {
            in_escape_sequence = true;
            current_cell += str[i];
            i++;
            continue;
        }

        if (in_escape_sequence) {
            current_cell += str[i];
            if ((str[i] >= L'A' && str[i] <= L'Z') || (str[i] >= L'a' && str[i] <= L'z')) {
                in_escape_sequence = false;
            }
            i++;
            continue;
        }

        if (StringMod::isViewable(str[i]) && !current_cell.empty()) {
            if (const int viewableChars = StringMod::viewableCharCount(current_cell); viewableChars != 0) {
                m_buffer[y][pos] = current_cell;
                pos++;
                current_cell.clear();
            }
            current_cell += str[i];
            i++;
        } else {
            current_cell += str[i];
            i++;
        }

        if (pos < m_width && (i == str.length() || StringMod::isViewable(str[i]) ||
                              StringMod::isHighSurrogate(str[i]))) {
            m_buffer[y][pos] = current_cell;
            pos++;
            current_cell.clear();
        }
    }

    if (!current_cell.empty() && pos < m_width) {
        m_buffer[y][pos] = current_cell;
    }
}

void Buffer::clear() {
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            m_buffer[y][x] = L' ';
        }
    }
}

int Buffer::getWidth() const {
    return m_width;
}

int Buffer::getHeight() const {
    return m_height;
}
