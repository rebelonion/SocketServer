#pragma once

#include "ThreadSafeVector.h"
#include "TUIItem.h"

class TextBox final : public TUIItem {
public:
    enum Direction {
        UP, DOWN
    };

    TextBox(int x, int y, int width, int height);

    bool update(double _, Buffer &buffer) override;

    void addLine(const std::wstring &line);

    void clear();

    void setDirection(Direction direction);

private:
    Direction m_direction = DOWN;
    ThreadSafeVector m_lines;
    bool m_redrawNeeded = true;
    bool m_fullRedrawNeeded = true;
};
