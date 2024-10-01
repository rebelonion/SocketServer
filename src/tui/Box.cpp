#include "Box.h"

#include "Drawable.h"

Box::Box(const int x, const int y, const int width, const int height): TUIItem(x, y, width, height) {
}

bool Box::update(double delta, Buffer &buffer) {
    [[likely]] if (!m_draw) {
        return false;
    }
    drawBox(buffer);
    for (const auto &crossbar: m_crossbars) {
        drawSplitBar(buffer, crossbar);
    }
    updateIntersections(buffer);

    m_draw = false;
    return true;
}

void Box::updateIntersections(Buffer &buffer) {
    std::vector<std::pair<int, int> > intersections;
    intersections.emplace_back(m_x, m_y);
    intersections.emplace_back(m_x + m_width - 1, m_y);
    intersections.emplace_back(m_x, m_y + m_height - 1);
    intersections.emplace_back(m_x + m_width - 1, m_y + m_height - 1);
    for (const auto &crossbar: m_crossbars) {
        intersections.emplace_back(m_x, crossbar);
        intersections.emplace_back(m_x + m_width - 1, crossbar);
    }

    for (const auto &[x, y]: intersections) {
        bool top = false, bottom = false, left = false, right = false;

        auto hasVerticalConnection = [](wchar_t ch) {
            return ch == Drawable::VERTICAL || ch == Drawable::TOP_LEFT || ch == Drawable::TOP_RIGHT
                   || ch == Drawable::BOTTOM_LEFT || ch == Drawable::BOTTOM_RIGHT || ch == Drawable::LEFT_TEE
                   || ch == Drawable::RIGHT_TEE || ch == Drawable::TOP_TEE || ch == Drawable::BOTTOM_TEE
                   || ch == Drawable::CROSS;
        };

        auto hasHorizontalConnection = [](const wchar_t ch) {
            return ch == Drawable::HORIZONTAL || ch == Drawable::TOP_LEFT || ch == Drawable::TOP_RIGHT
                   || ch == Drawable::BOTTOM_LEFT || ch == Drawable::BOTTOM_RIGHT || ch == Drawable::LEFT_TEE
                   || ch == Drawable::RIGHT_TEE || ch == Drawable::TOP_TEE || ch == Drawable::BOTTOM_TEE
                   || ch == Drawable::CROSS;
        };

        if (y > 0 && hasVerticalConnection(buffer.getLine(y - 1)[x])) top = true;
        if (y < buffer.getHeight() - 1 && hasVerticalConnection(buffer.getLine(y + 1)[x])) bottom = true;
        if (x > 0 && hasHorizontalConnection(buffer.getLine(y)[x - 1])) left = true;
        if (x < buffer.getWidth() - 1 && hasHorizontalConnection(buffer.getLine(y)[x + 1])) right = true;

        wchar_t intersectionChar;

        if (top && bottom && left && right) intersectionChar = Drawable::CROSS;
        else if (top && bottom && right) intersectionChar = Drawable::RIGHT_TEE;
        else if (top && bottom && left) intersectionChar = Drawable::LEFT_TEE;
        else if (top && left && right) intersectionChar = Drawable::BOTTOM_TEE;
        else if (bottom && left && right) intersectionChar = Drawable::TOP_TEE;
        else if (top && right) intersectionChar = Drawable::BOTTOM_LEFT;
        else if (top && left) intersectionChar = Drawable::BOTTOM_RIGHT;
        else if (bottom && right) intersectionChar = Drawable::TOP_LEFT;
        else if (bottom && left) intersectionChar = Drawable::TOP_RIGHT;
        else if (top || bottom) intersectionChar = Drawable::VERTICAL;
        else if (left || right) intersectionChar = Drawable::HORIZONTAL;
        else intersectionChar = L' ';

        buffer.draw(x, y, std::wstring(1, intersectionChar));
    }
}

void Box::addCrossbar(const int y) {
    if (y < 1 || y >= m_height - 1 || std::ranges::find(m_crossbars, y) != m_crossbars.end()) {
        return;
    }
    m_crossbars.push_back(y);
}

void Box::drawSplitBar(Buffer &buffer, const int y) const {
    buffer.draw(m_x, y, &Drawable::RIGHT_TEE);
    buffer.draw(m_x + m_width - 1, y, &Drawable::LEFT_TEE);
    for (int x = m_x; x < m_x + m_width - 1; ++x) {
        buffer.draw(x, y, &Drawable::HORIZONTAL);
    }
}

void Box::drawBox(Buffer &buffer) const {
    for (int x = m_x; x < m_x + m_width; ++x) {
        buffer.draw(x, m_y, &Drawable::HORIZONTAL);
        buffer.draw(x, m_y + m_height - 1, &Drawable::HORIZONTAL);
    }

    for (int y = m_y; y < m_y + m_height - 1; ++y) {
        buffer.draw(m_x, y, &Drawable::VERTICAL);
        buffer.draw(m_x + m_width - 1, y, &Drawable::VERTICAL);
    }

    buffer.draw(m_x, m_y, &Drawable::TOP_LEFT);
    buffer.draw(m_x + m_width - 1, m_y, &Drawable::TOP_RIGHT);
    buffer.draw(m_x, m_y + m_height - 1, &Drawable::BOTTOM_LEFT);
    buffer.draw(m_x + m_width - 1, m_y + m_height - 1, &Drawable::BOTTOM_RIGHT);
}
