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
#endif
#include <stdexcept>

bool UserInput::inputAvailable() {
#ifdef _WIN32
    return _kbhit();
#else
        timeval tv{};
        fd_set fds;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);
        return FD_ISSET(STDIN_FILENO, &fds);
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
            } else if (ch == L'\b') {
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
            } else if (ch != 0) {
                return oldVal + ch;
            }
        }
    }

    SetConsoleMode(hStdin, mode);
    return std::nullopt;
#else
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    char buf[8] = {0};
    int bytesRead = 0;

    char c = getch();
    buf[bytesRead++] = c;

    if (c == 10) {
        return oldVal;
    } else if (c == 127) {
        if (!oldVal.empty()) {
            oldVal.pop_back();
            return oldVal;
        }
    } else if ((c & 0xC0) == 0xC0) {
        while (bytesRead < 4 && (c & 0xC0) == 0x80) {
            c = getch();
            buf[bytesRead++] = c;
        }

        std::string mbStr(buf, bytesRead);
        std::wstring wStr = converter.from_bytes(mbStr);

        if (!wStr.empty()) {
            return oldVal + wStr[0];
        }
    } else if (c >= 32) {
        return oldVal + static_cast<wchar_t>(c);
    }

    return std::nullopt;
#endif
}

char UserInput::getch() {
#ifndef _WIN32
        char buf = 0;
        termios old = {0};
        fflush(stdout);
        if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0) perror("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
        return buf;
#else
    throw std::runtime_error("getch() is not needed on Windows");
#endif
}
