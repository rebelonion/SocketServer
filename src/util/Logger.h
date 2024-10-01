#pragma once


#include <functional>
#include <source_location>
#include <string>

class Logger {
public:

    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error
    };

    using LogFunction = std::function<void(const std::wstring&)>;

    explicit Logger(LogLevel minLevel = LogLevel::Warning);

    void log(LogLevel level, const std::wstring &message, const std::source_location &location = std::source_location::current()) const;

    void log(LogLevel level, const std::string &message, const std::source_location &location = std::source_location::current()) const;

    void setLogLevel(LogLevel level);

    void setCustomLogFunction(LogFunction customLogFunc);

private:
    LogLevel m_minLevel;
    LogFunction m_customLogFunc;

    void logDebug(const std::wstring &message, const std::source_location &location) const;

    void logInfo(const std::wstring &message, const std::source_location &location) const;

    void logWarning(const std::wstring &message, const std::source_location &location) const;

    void logError(const std::wstring &message, const std::source_location &location) const;

    void logMessage(const std::wstring &message, unsigned int color, const std::source_location &location) const;

    static constexpr unsigned int m_debugColor = 32;
    static constexpr unsigned int m_infoColor = 36;
    static constexpr unsigned int m_warningColor = 33;
    static constexpr unsigned int m_errorColor = 31;
};
