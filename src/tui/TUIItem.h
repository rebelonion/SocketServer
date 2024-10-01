#pragma once

#include "Buffer.h"

class TUIItem {
public:
    TUIItem(const int x, const int y, const int width, const int height): m_x(x), m_y(y), m_width(width),
                                                                          m_height(height) {
    }

    virtual ~TUIItem() = default;

    virtual bool update(double delta, Buffer &buffer) = 0;

protected:
    int m_x, m_y, m_width, m_height;
};
