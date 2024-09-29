#include "Buffer.h"

Buffer::Buffer(const int width, const int height) {
    m_width = width;
    m_height = height;
    m_buffer.resize(m_height, std::wstring(m_width, L' '));
}

std::wstring Buffer::getLine(const int y) const {
    return m_buffer[y];
}

void Buffer::draw(const int x, const int y, const std::wstring &str) {
    if (y >= 0 && y < m_height) {
        if (const int len = std::min(static_cast<int>(str.length()), m_width - x); len > 0) {
            m_buffer[y].replace(x, len, str.substr(0, len));
        }
    }
}

void Buffer::clear() {
    for (int y = 0; y < m_height; ++y) {
        m_buffer[y].assign(m_width, L' ');
    }
}

int Buffer::getWidth() const {
    return m_width;
}

int Buffer::getHeight() const {
    return m_height;
}
