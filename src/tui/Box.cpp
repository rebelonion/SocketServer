#include "Box.h"

#include "Drawable.h"

Box::Box(const int x, const int y, const int width, const int height): TUIItem(x, y, width, height) {
}

bool Box::update(double delta, Buffer &buffer) {
    if (!m_draw) {
        return false;
    }
    drawBox(buffer);
    for (const auto &crossbar: m_crossbars) {
        drawSplitBar(buffer, crossbar);
    }

    m_draw = false;
    return true;
}

void Box::addCrossbar(const int y) {
    if (y < 1 || y >= m_height - 1 || std::ranges::find(m_crossbars, y) != m_crossbars.end()) {
        return;
    }
    m_crossbars.push_back(y);
}

void Box::drawSplitBar(Buffer &buffer, const int y) const {
    buffer.draw(0, y, &Drawable::RIGHT_TEE);
    buffer.draw(m_width - 1, y, &Drawable::LEFT_TEE);
    for (int x = 1; x < m_width - 1; ++x) {
        buffer.draw(x, y, &Drawable::HORIZONTAL);
    }
}

void Box::drawBox(Buffer &buffer) const {
    for (int x = 0; x < m_width; ++x) {
        buffer.draw(x, 0, &Drawable::HORIZONTAL);
        buffer.draw(x, m_height - 1, &Drawable::HORIZONTAL);
    }

    for (int y = 1; y < m_height - 1; ++y) {
        buffer.draw(0, y, &Drawable::VERTICAL);
        buffer.draw(m_width - 1, y, &Drawable::VERTICAL);
    }

    buffer.draw(0, 0, &Drawable::TOP_LEFT);
    buffer.draw(m_width - 1, 0, &Drawable::TOP_RIGHT);
    buffer.draw(0, m_height - 1, &Drawable::BOTTOM_LEFT);
    buffer.draw(m_width - 1, m_height - 1, &Drawable::BOTTOM_RIGHT);
}
