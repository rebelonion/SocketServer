#include "UserInput.h"
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <codecvt>
#include <locale>
#include <fcntl.h>
#endif
#include <stdexcept>

bool UserInput::inputAvailable() {
#ifdef _WIN32
    return _kbhit();
#else
        if (!is_raw_mode) {
            enableRawMode();
        }
        fd_set fds;
        timeval tv = {0, 0};
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
#endif
}

std::optional<std::wstring> UserInput::getInput(std::wstring oldVal) {
#ifdef _WIN32
    const auto hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & ~ENABLE_ECHO_INPUT);

    INPUT_RECORD record;
    DWORD count;

    while (ReadConsoleInputW(hStdin, &record, 1, &count) && count > 0) {
        if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
            if (const WCHAR ch = record.Event.KeyEvent.uChar.UnicodeChar; ch == L'\r' || ch == L'\n') {
                SetConsoleMode(hStdin, mode);
                return oldVal;
            } else {
                if (ch == L'\b') {
                    if (!oldVal.empty()) {
                        if (oldVal.length() >= 2 &&
                            (oldVal[oldVal.length() - 2] & 0xFC00) == 0xD800 &&
                            (oldVal[oldVal.length() - 1] & 0xFC00) == 0xDC00) {
                            // This is a surrogate pair, remove both characters
                            oldVal.pop_back();
                            oldVal.pop_back();
                        } else {
                            oldVal.pop_back();
                        }
                        return oldVal;
                    }
                    // empty string
                    return std::nullopt;
                }
                if (ch != 0) {
                    return oldVal + ch;
                }
            }
        }
    }

    SetConsoleMode(hStdin, mode);
    return std::nullopt;
#else

    char buf[8] = {};
    int bytesRead = 0;
    char c = getch();
    buf[bytesRead++] = c;
    if (c == 10) {
        return oldVal;
    } else if (c == 127 || c == 8) {  // Check for both DEL and BS
        if (!oldVal.empty()) {
            oldVal.pop_back();
            return oldVal;
        }
        return std::nullopt;
    } else if ((c & 0x80) == 0x80) {  // multi-byte character
        int expectedBytes;
        if ((c & 0xE0) == 0xC0) expectedBytes = 2;
        else if ((c & 0xF0) == 0xE0) expectedBytes = 3;
        else if ((c & 0xF8) == 0xF0) expectedBytes = 4;
        else return std::nullopt;  // Invalid UTF-8 start byte

        while (bytesRead < expectedBytes) {
            c = getch();
            if ((c & 0xC0) != 0x80) return std::nullopt;  // Invalid continuation byte
            buf[bytesRead++] = c;
        }
    }

    if (bytesRead > 0) {
        const std::string mbStr(buf, bytesRead);
        if (const std::wstring wStr = StringMod::toWString(mbStr); !wStr.empty()) {
            return oldVal + wStr;
        }
    }
    return std::nullopt;
#endif
}

UserInput &UserInput::getInstance() {
    static UserInput instance;
    return instance;
}

#ifndef _WIN32
char UserInput::getch() {
    if (!is_raw_mode) {
        enableRawMode();
    }
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        return c;
    }
    return '\0';
}

void UserInput::enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit([]() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &getInstance().orig_termios); });

    auto raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    const int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    is_raw_mode = true;
}
#endif
