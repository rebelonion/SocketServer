#include "Buffer.h"

#include "Drawable.h"
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
    if (!isValidPosition(y)) return;

    int pos = y * m_width + x;
    size_t i = 0;
    bool in_escape_sequence = false;
    std::wstring current_cell;

    while (i < str.length() && pos < m_width * m_height) {
        if (str[i] == Drawable::ESCAPE_SEQUENCE) {
            in_escape_sequence = true;
            current_cell += str[i];
            i++;
            continue;
        }

        if (StringMod::isSingleUnicodeWideCharacter(str[i])) {
            m_buffer[pos / m_width][pos % m_width] = L"";
            pos++;
        }
        if (in_escape_sequence) {
            handleEscapeSequence(current_cell, str, i, in_escape_sequence);
        } else if (StringMod::isViewable(str[i]) && !current_cell.empty()) {
            handleViewableCharacter(current_cell, str, i, pos);
        } else {
            handleNonViewableCharacter(current_cell, str, i);
        }

        finalizeCell(current_cell, pos, str, i);
    }

    if (!current_cell.empty() && pos < m_width * m_height) {
        m_buffer[pos / m_width][pos % m_width] = current_cell;
    }

    if (y == 28) {
        auto line = getLine(y);
        auto line2 = getLine(y + 1);
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

bool Buffer::isValidPosition(int y) const {
    return y >= 0 && y < m_height;
}

void Buffer::handleEscapeSequence(std::wstring &current_cell, const std::wstring_view &str, size_t &i,
                                  bool &in_escape_sequence) {
    current_cell += str[i];
    if ((str[i] >= L'A' && str[i] <= L'Z') || (str[i] >= L'a' && str[i] <= L'z')) {
        in_escape_sequence = false;
    }
    i++;
}

void Buffer::handleViewableCharacter(std::wstring &current_cell, const std::wstring_view &str, size_t &i, int &pos) {
    if (const int viewableChars = StringMod::viewableCharCount(current_cell); viewableChars != 0) {
        m_buffer[pos / m_width][pos % m_width] = current_cell;
        pos++;
        current_cell.clear();
    }
    current_cell += str[i];
    i++;
}

void Buffer::handleNonViewableCharacter(std::wstring &current_cell, const std::wstring_view &str, size_t &i) {
    current_cell += str[i];
    i++;
}

void Buffer::finalizeCell(std::wstring &current_cell, int &pos, const std::wstring_view &str, const size_t i) {
    if (pos < m_width && (i == str.length() || StringMod::isViewable(str[i]) || StringMod::isHighSurrogate(str[i]))) {
        m_buffer[pos / m_width][pos % m_width] = current_cell;
        pos++;
        current_cell.clear();
    }
}
