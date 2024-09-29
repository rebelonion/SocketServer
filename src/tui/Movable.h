#pragma once

#include <string>
#include <vector>

#include "Buffer.h"
#include "TUIItem.h"

class Movable final : public TUIItem {
public:
    Movable(const std::vector<std::vector<std::wstring> > &frames, double delay, int x,
            int y, int w, int h);

    bool update(double delta, Buffer &buffer) override;

private:
    std::vector<std::vector<std::wstring> > m_frames;
    int m_currentFrame = 0;
    double m_lastFrameTime = 0.0;
    double m_frameDelay = 0.1;
};
