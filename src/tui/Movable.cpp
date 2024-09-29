#include "Movable.h"

Movable::Movable(const std::vector<std::vector<std::wstring>> &frames, const double delay, const int x, const int y,
    const int w, const int h): TUIItem(x, y, w, h), m_frames(frames), m_frameDelay(delay) {
}

bool Movable::update(const double delta, Buffer &buffer) {
    if (delta - m_lastFrameTime > m_frameDelay) {
        m_lastFrameTime = delta;
        m_currentFrame = (m_currentFrame + 1) % static_cast<int>(m_frames.size());
        const auto toDraw = m_frames[m_currentFrame];
        for (auto line = 0; line < toDraw.size(); ++line) {
            buffer.draw(m_x, m_y + line, toDraw[line]);
        }
        return true;
    }
    return false;
}
