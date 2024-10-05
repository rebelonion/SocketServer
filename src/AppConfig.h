#pragma once

#include <memory>
#include <string>

#include "ServerSocket.h"
#include "Socket.h"

class AppConfig {
public:
    AppConfig();

    void parseCommandLine(int argc, char *argv[]);

    void validate() const;

    std::unique_ptr<Socket> appSocket;
    std::string port;
    std::string address;
    bool fun = false;

private:
    void parseArgument(std::string_view arg, const char *nextArg);

    static void parseLogLevel(const std::string &logLevel);
};
