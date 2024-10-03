#include <chrono>
#include <iostream>
#include <filesystem>
#include <source_location>

#include "Logger.h"
#include "StringMod.h"

Logger::Logger(const LogLevel minLevel) {
    m_minLevel = minLevel;
}

void Logger::log(const LogLevel level, const std::string &message, const std::source_location &location) const {
    log(level, StringMod::toWString(message), location);
}

void Logger::log(const LogLevel level, const std::wstring &message, const std::source_location &location) const {
    switch (level) {
            using enum LogLevel;
        case Debug:
            logDebug(message, location);
            break;
        case Info:
            logInfo(message, location);
            break;
        case Warning:
            logWarning(message, location);
            break;
        case Error:
            logError(message, location);
            break;
    }
}

void Logger::setCustomLogFunction(LogFunction customLogFunc) {
    m_customLogFunc = std::move(customLogFunc);
}

void Logger::setLogLevel(const LogLevel level) {
    m_minLevel = level;
}

void Logger::logDebug(const std::wstring &message, const std::source_location &location) const {
    if (m_minLevel > LogLevel::Debug) {
        return;
    }
    logMessage(message, m_debugColor, location);
}

void Logger::logInfo(const std::wstring &message, const std::source_location &location) const {
    if (m_minLevel > LogLevel::Info) {
        return;
    }
    logMessage(message, m_infoColor, location);
}

void Logger::logWarning(const std::wstring &message, const std::source_location &location) const {
    if (m_minLevel > LogLevel::Warning) {
        return;
    }
    logMessage(message, m_warningColor, location);
}

void Logger::logError(const std::wstring &message, const std::source_location &location) const {
    if (m_minLevel > LogLevel::Error) {
        return;
    }
    logMessage(message, m_errorColor, location);
}

void Logger::logMessage(const std::wstring &message, const unsigned int color,
                        const std::source_location &location) const {
    const auto now = std::chrono::system_clock::now();
    auto local_time = std::chrono::zoned_time{std::chrono::current_zone(), now};
    const std::wstring timeString = std::format(L"{:%Y-%m-%d %H:%M:%S}", local_time);
    std::wstring logMessage;
    if (m_minLevel == LogLevel::Debug) {
        const std::wstring locationString = std::format(L"[{}:{}] ",
                                                        StringMod::toWString(
                                                            std::filesystem::path(location.file_name()).filename().
                                                            string()),
                                                        location.line());
        logMessage = StringMod::color(L"[" + timeString + L"] " + locationString + message, color);
    } else {
        logMessage = StringMod::color(L"[" + timeString + L"] " + message, color);
    }
    if (m_customLogFunc) {
        m_customLogFunc(logMessage);
    } else {
        std::wcout << logMessage;
    }
}
