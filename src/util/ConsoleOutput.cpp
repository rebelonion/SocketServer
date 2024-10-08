#include "ConsoleOutput.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#else
#include <codecvt>
#endif

ConsoleOutput::ConsoleOutput() {
#ifdef _WIN32
        _setmode(_fileno(stdout), _O_WTEXT);
#endif
}

ConsoleOutput & ConsoleOutput::operator<<(const std::wstring &value) {
#ifdef _WIN32
        std::wcout << value;
#else
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::cout << converter.to_bytes(value);
#endif
        return *this;
}

ConsoleOutput & ConsoleOutput::operator<<(std::ostream &(*manipulator)(std::ostream &)) {
#ifdef _WIN32
        std::wcout << manipulator;
#else
        std::cout << manipulator;
#endif
        return *this;
}

void ConsoleOutput::flush() {
#ifdef _WIN32
        std::wcout.flush();
#else
        std::cout.flush();
#endif
}
