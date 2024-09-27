#include <chrono>
#include <iostream>
#include "Logger.h"

Logger::Logger(const Logger::LogLevel minLevel) {
    m_minLevel = minLevel;
}


void Logger::log(const LogLevel level, const std::string &message) const {
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

void Logger::setLogLevel(const Logger::LogLevel level) {
    m_minLevel = level;
}

void Logger::logDebug(const std::string &message) const {
    if (m_minLevel > LogLevel::Debug) {
        return;
    }
    logMessage(message, m_debugColor);
}

void Logger::logInfo(const std::string &message) const {
    if (m_minLevel > LogLevel::Info) {
        return;
    }
    logMessage(message, m_infoColor);
}

void Logger::logWarning(const std::string &message) const {
    if (m_minLevel > LogLevel::Warning) {
        return;
    }
    logMessage(message, m_warningColor);
}

void Logger::logError(const std::string &message) const {
    if (m_minLevel > LogLevel::Error) {
        return;
    }
    logMessage(message, m_errorColor);
}

void Logger::logMessage(const std::string &message, const unsigned int color) {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::cout << "\033[" << color << "m";
    std::cout << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] "
              << message << std::endl;
    std::cout << "\033[0m";
}