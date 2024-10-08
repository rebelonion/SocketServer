#include <iostream>
#include <utility>
#include <vector>
#include <memory>

#include "AppConfig.h"
#include "ChatApplication.h"
#include "Logger.h"
#include "Socket.h"
#include "Globals.h"
#include "TUI.h"

int main(const int argc, char *argv[]) {
    try {
        AppConfig config;
        config.parseCommandLine(argc, argv);
        config.validate();

        const bool isServer = config.appSocket->isServer();
        ChatApplication app(std::move(config.appSocket), config.port, config.address);
        app.run(config.fun);
        logger.log(Logger::LogLevel::Debug, std::format("{} stopped", isServer ? "Server" : "Client"));
    } catch (const std::exception &e) {
        logger.log(Logger::LogLevel::Error, "Error: " + std::string(e.what()));
        return 1;
    }
    return 0;
}
