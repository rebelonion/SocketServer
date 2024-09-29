#include <chrono>
#include <iostream>
#include "Logger.h"

#include "StringMod.h"

Logger::Logger(const Logger::LogLevel minLevel) {
    m_minLevel = minLevel;
}

void Logger::log(const LogLevel level, const std::string &message) const {
    log(level, StringMod::toWString(message));
}

void Logger::log(const LogLevel level, const std::wstring &message) const {
    switch (level) {
        case LogLevel::Debug:
            logDebug(message);
            break;
        case LogLevel::Info:
            logInfo(message);
            break;
        case LogLevel::Warning:
            logWarning(message);
            break;
        case LogLevel::Error:
            logError(message);
            break;
    }
}

void Logger::setCustomLogFunction(LogFunction customLogFunc) {
    m_customLogFunc = std::move(customLogFunc);
}

void Logger::setLogLevel(const LogLevel level) {
    m_minLevel = level;
}

void Logger::logDebug(const std::wstring &message) const {
    if (m_minLevel > LogLevel::Debug) {
        return;
    }
    logMessage(message, m_debugColor);
}

void Logger::logInfo(const std::wstring &message) const {
    if (m_minLevel > LogLevel::Info) {
        return;
    }
    logMessage(message, m_infoColor);
}

void Logger::logWarning(const std::wstring &message) const {
    if (m_minLevel > LogLevel::Warning) {
        return;
    }
    logMessage(message, m_warningColor);
}

void Logger::logError(const std::wstring &message) const {
    if (m_minLevel > LogLevel::Error) {
        return;
    }
    logMessage(message, m_errorColor);
}

void Logger::logMessage(const std::wstring &message, const unsigned int color) const {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::wostringstream woss;
    woss << std::put_time(std::localtime(&time), L"%Y-%m-%d %H:%M:%S");
    const std::wstring timeString = woss.str();
    const std::wstring logMessage = StringMod::color(L"[" + timeString + L"] " + message, color);
    if (m_customLogFunc) {
        m_customLogFunc(logMessage);
    } else {
        std::wcout << logMessage;
    }

}