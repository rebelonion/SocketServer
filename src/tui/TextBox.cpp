#include "TextBox.h"

#include "StringMod.h"

TextBox::TextBox(const int x, const int y, const int width, const int height): TUIItem(x, y, width, height) {
}

bool TextBox::update(double _, Buffer &buffer) {
    if (!m_redrawNeeded) return false;
    [[unlikely]] if (m_fullRedrawNeeded) {
        for (int i = 0; i < m_height; ++i) {
            buffer.draw(m_x, m_y + i, std::wstring(m_width, L' '));
        }
        m_fullRedrawNeeded = false;
    }
    const int startY = (m_direction == Direction::DOWN) ? m_y + m_height - 1 : m_y;
    const int endY = (m_direction == Direction::DOWN) ? m_y - 1 : m_y + m_height;
    const int step = (m_direction == Direction::DOWN) ? -1 : 1;
    const std::wstring emptyLine(m_width, L' ');

    int currentY = startY;
    size_t lineIndex = 0;
    const size_t totalLines = m_lines.size();

    for (; currentY != endY; currentY += step) {
        if (lineIndex >= totalLines) break;

        std::wstring line = (m_direction == Direction::DOWN)
                                ? m_lines.get(static_cast<int>(totalLines - lineIndex - 1))
                                : m_lines.get(static_cast<int>(lineIndex));
        while (StringMod::viewableCharCount(line) > static_cast<size_t>(m_width)) {
            line.pop_back();
        }
        buffer.draw(m_x, currentY, emptyLine);
        buffer.draw(m_x, currentY, line);
        lineIndex++;
    }
    m_redrawNeeded = false;
    return true;
}

void TextBox::addLine(const std::wstring &line) {
    m_lines.add(line);
    if (m_lines.size() > static_cast<size_t>(m_height)) {
        m_lines.remove(0);
    }
    m_redrawNeeded = true;
}

void TextBox::clear() {
    m_lines.clear();
    m_fullRedrawNeeded = true;
    m_redrawNeeded = true;
}

void TextBox::setDirection(const Direction direction) {
    m_direction = direction;
    m_redrawNeeded = true;
}
