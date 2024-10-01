#pragma once

#include "TUIItem.h"


class Box final : public TUIItem {
public:
    Box(int x, int y, int width, int height);

    bool update(double delta, Buffer &buffer) override;

    void updateIntersections(Buffer &buffer);

    void addCrossbar(int y);

private:
    std::vector<int> m_crossbars;
    bool m_draw = true;

    void drawSplitBar(Buffer &buffer, int y) const;

    void drawBox(Buffer &buffer) const;
};
