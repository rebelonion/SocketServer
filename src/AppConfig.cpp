#include "AppConfig.h"
#include "ClientSocket.h"
#include "Errors.h"
#include "Globals.h"

AppConfig::AppConfig() = default;

void AppConfig::parseCommandLine(const int argc, char *argv[]) {
    int i = 1;
    while (i < argc) {
        const char *currentArg = argv[i];
        const char *nextArg = (i + 1 < argc) ? argv[i + 1] : nullptr;

        parseArgument(currentArg, nextArg);

        if (nextArg && (std::string(currentArg) == "-a" ||
                        std::string(currentArg) == "-p" ||
                        std::string(currentArg) == "-l")) {
            i += 2;
        } else {
            ++i;
        }
    }
}

void AppConfig::validate() const {
    if (!appSocket) {
        throw StartupError("Either server or client must be specified");
    }
    if (port.empty()) {
        throw StartupError("Port must be specified");
    }
    if (address.empty() && !appSocket->isServer()) {
        throw StartupError("Address must be specified for client");
    }
}

void AppConfig::parseArgument(const std::string_view arg, const char *nextArg) {
    if (arg == "-s" || arg == "--server") {
        appSocket = std::make_unique<ServerSocket>();
    } else if (arg == "-c" || arg == "--client") {
        appSocket = std::make_unique<ClientSocket>();
    } else if (arg == "-a" || arg == "--address") {
        if (!nextArg) throw StartupError("Address argument is missing");
        address = nextArg;
    } else if (arg == "-p" || arg == "--port") {
        if (!nextArg) throw StartupError("Port argument is missing");
        port = nextArg;
    } else if (arg == "-f" || arg == "--fun") {
        fun = true;
    } else if (arg == "-l" || arg == "--log-level") {
        if (!nextArg) throw StartupError("Log level argument is missing");
        parseLogLevel(nextArg);
    }
}

void AppConfig::parseLogLevel(const std::string &logLevel) {
    switch (logLevel[0]) {
            using enum Logger::LogLevel;
        case 'd':
        case 'D': logger.setLogLevel(Debug);
            break;
        case 'i':
        case 'I': logger.setLogLevel(Info);
            break;
        case 'w':
        case 'W': logger.setLogLevel(Warning);
            break;
        case 'e':
        case 'E': logger.setLogLevel(Error);
            break;
        default: throw StartupError("Invalid log level");
    }
}
