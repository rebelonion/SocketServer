#include "TUI.h"
#ifdef _WIN32
#include <fcntl.h>
#include <windows.h>
#elif __linux__
#include <locale>
#include <clocale>
#endif
#include <thread>
#include <cstdlib>
#include <iostream>
#include "Globals.h"
#include "StringMod.h"

TUI::TUI(const int w, const int h): m_buffer(w, h), m_prevBuffer(w, h), m_width(w), m_height(h) {
#ifdef _WIN32
    _setmode(_fileno(stdout), _O_U16TEXT);
#elif __linux__
        std::locale::global(std::locale("en_US.UTF-8"));
        std::wcout.imbue(std::locale());
#endif
    clearScreen();
    hideCursor();
    quickFullRender();
}

TUI::~TUI() {
    showCursor();
}

void TUI::draw(const int x, const int y, const std::wstring &str) {
    if (y >= 0 && y < m_height) {
        if (const int len = std::min(static_cast<int>(str.length()), m_width - x); len > 0) {
            m_buffer.draw(x, y, str.substr(0, len));
            m_redrawNeeded = true;
        }
    }
}

void TUI::render() {
    m_lastFrameTime = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    updateTItems();
    if (m_redrawNeeded) {
        for (int y = 0; y < m_height; ++y) {
            if (m_buffer.getLine(y) != m_prevBuffer.getLine(y)) {
                moveCursor(0, y);
                std::wcout << m_buffer.getLine(y);
            }
        }
        std::wcout.flush();
        m_prevBuffer = m_buffer;
        m_redrawNeeded = false;
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void TUI::quickFullRender() {
#ifdef _WIN32
    updateTItems();
    const auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        logger.log(Logger::LogLevel::Error, std::format(L"Failed to get console handle. Error: {}", GetLastError()));
        return;
    }

    constexpr COORD topLeft = {0, 0};
    SetConsoleCursorPosition(hConsole, topLeft);

    for (int y = 0; y < m_buffer.getHeight(); ++y) {
        std::wstring line = m_buffer.getLine(y);
        if (y < m_buffer.getHeight() - 1) {
            line += L'\n';
        }

        DWORD written;
        WriteConsoleW(hConsole, line.c_str(), line.length(), &written, nullptr);
    }

    m_prevBuffer = m_buffer;
    m_redrawNeeded = false;
#endif
}

int TUI::getWidth() const {
    return m_width;
}

int TUI::getHeight() const {
    return m_height;
}

void TUI::offerTItem(std::shared_ptr<TUIItem> item) {
    m_TUIItems.push_back(std::move(item));
}

void TUI::removeTItem(const TUIItem *item) {
    const auto it = std::ranges::find_if(m_TUIItems,
                                         [item](const auto &ptr) { return ptr.get() == item; });
    if (it != m_TUIItems.end()) {
        m_TUIItems.erase(it);
    }
}

void TUI::updateTItems() {
    for (const auto &item: m_TUIItems) {
        m_redrawNeeded = item->update(m_lastFrameTime, m_buffer) || m_redrawNeeded;
    }
}

void TUI::clearScreen() {
    std::wcout << L"\033[2J\033[H";
}

void TUI::moveCursor(const int x, const int y) {
    std::wcout << std::format(L"\033[{};{}H", y + 1, x + 1);
}

void TUI::hideCursor() {
    std::wcout << L"\033[?25l";
}

void TUI::showCursor() {
    std::wcout << L"\033[?25h";
}