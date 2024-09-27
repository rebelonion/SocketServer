#pragma once


#include <string>

class Logger {
public:

    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error
    };

    explicit Logger(LogLevel minLevel = LogLevel::Warning);

    void log(LogLevel level, const std::string &message) const;

    void setLogLevel(LogLevel level);

private:
    LogLevel m_minLevel;

    void logDebug(const std::string &message) const;

    void logInfo(const std::string &message) const;

    void logWarning(const std::string &message) const;

    void logError(const std::string &message) const;

    static void logMessage(const std::string &message, unsigned int color);

    static constexpr unsigned int m_debugColor = 32;
    static constexpr unsigned int m_infoColor = 36;
    static constexpr unsigned int m_warningColor = 33;
    static constexpr unsigned int m_errorColor = 31;
};
