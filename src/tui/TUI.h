#pragma once

#include <vector>
#include <chrono>
#include <format>

#include "Buffer.h"
#include "TUIItem.h"

class TUI {
public:
    TUI(int w, int h);

    ~TUI();

    void draw(int x, int y, std::wstring_view str);

    void render();

    void quickFullRender();

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

    void offerTItem(std::shared_ptr<TUIItem> item);

    void removeTItem(const TUIItem *item);

    void updateTItems();

private:
    Buffer m_buffer;
    Buffer m_prevBuffer;
    int m_width;
    int m_height;
    bool m_redrawNeeded = true;
    double m_lastFrameTime = 0.0;
    std::vector<std::shared_ptr<TUIItem> > m_TUIItems;

    static void clearScreen();

    static void moveCursor(int x, int y);

    static void hideCursor();

    static void showCursor();
};
