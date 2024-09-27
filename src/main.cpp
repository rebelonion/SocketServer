#include <iostream>
#include <utility>
#include <vector>
#include <memory>
#include "Logger.h"
#include "Socket.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "Globals.h"
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

void runServer(std::string port);

void runClient(const std::string &port, const std::string &address);

std::unique_ptr<Socket> appSocket = nullptr;

int main(const int argc, char *argv[]) {
    try {
        std::string port;
        std::string address;
        for (int i = 1; i < argc; i++) {
            if (std::string arg = argv[i]; arg == "-s" || arg == "--server") {
                appSocket = std::make_unique<ServerSocket>();
            } else if (arg == "-c" || arg == "--client") {
                appSocket = std::make_unique<ClientSocket>();
            } else if (arg == "-a" || arg == "--address") {
                if (i + 1 < argc) {
                    address = argv[++i];
                } else {
                    throw std::runtime_error("Address argument is missing");
                }
            } else if (arg == "-p" || arg == "--port") {
                if (i + 1 < argc) {
                    port = argv[++i];
                } else {
                    throw std::runtime_error("Port argument is missing");
                }
            } else if (arg == "-l" || arg == "--log-level") {
                if (i + 1 < argc) {
                    switch (std::string logLevel = argv[++i]; logLevel[0]) {
                        case 'd':
                        case 'D':
                            logger.setLogLevel(Logger::LogLevel::Debug);
                            break;
                        case 'i':
                        case 'I':
                            logger.setLogLevel(Logger::LogLevel::Info);
                            break;
                        case 'w':
                        case 'W':
                            logger.setLogLevel(Logger::LogLevel::Warning);
                            break;
                        case 'e':
                        case 'E':
                            logger.setLogLevel(Logger::LogLevel::Error);
                            break;
                        default:
                            throw std::runtime_error("Invalid log level");
                    }
                }
            }
        }

        if (appSocket == nullptr) {
            throw std::runtime_error("Either server or client must be specified");
        }

        if (port.empty()) {
            throw std::runtime_error("Port must be specified");
        }

        if (address.empty() && !appSocket->isServer()) {
            throw std::runtime_error("Address must be specified for client");
        }

        logger.log(Logger::LogLevel::Debug, "Address: " + address);
        logger.log(Logger::LogLevel::Debug, "Port: " + port);

        if (appSocket->isServer()) {
            runServer(port);
            logger.log(Logger::LogLevel::Debug, "Server stopped");
        } else {
            runClient(port, address);
            logger.log(Logger::LogLevel::Debug, "Client stopped");
        }
    } catch (const std::exception &e) {
        logger.log(Logger::LogLevel::Error, "Error: " + std::string(e.what()));
        return 1;
    }
    return 0;
}
/*
void userInputThread(Socket &client, const std::stop_token& stoken) {
    while (!stoken.stop_requested()) {
        std::string user_input;
        std::getline(std::cin, user_input);

        if (user_input == "quit" || user_input == "exit") {
            shouldExit = true;
            break;
        }

        client.sendMessage(user_input, -1);
    }
}*/
bool inputAvailable() {
#ifdef _WIN32
    return _kbhit();
#else
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
#endif
}
void userInputThread(Socket &client, const std::stop_token& stoken) {
    std::string user_input;

    while (!stoken.stop_requested()) {
        if (inputAvailable()) {
            int c = _getch();
            if (c == 13) {
                std::cout << std::endl;
                if (user_input == "quit" || user_input == "exit") {
                    shouldExit = true;
                    break;
                }
                if (!user_input.empty()) {
                    client.sendMessage(user_input, -1);
                    user_input.clear();
                }
            } else if (c == 8) {
                if (!user_input.empty()) {
                    user_input.pop_back();
                    std::cout << "\b \b";
                }
            } else if (c >= 32 && c <= 126) {
                user_input += static_cast<char>(c);
                std::cout << static_cast<char>(c);
            }
            std::cout.flush();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void runServer(std::string port) {
    MessageQueue receivedMessages;
    const auto server = dynamic_cast<ServerSocket *>(appSocket.get());
    if (!server) {
        throw std::runtime_error("ServerSocket cast failed");
    }
    server->initSocket(std::move(port));

    std::jthread listener(&Socket::listenThread, std::ref(*server), std::ref(receivedMessages));
    std::stop_token stopToken = listener.get_stop_token();
    std::jthread inputThread(&userInputThread, std::ref(*server), std::ref(stopToken));

    while (!stopToken.stop_requested() && !shouldExit) {
        if (const auto message = receivedMessages.pop(stopToken)) {
            std::cout << *message << std::endl;
        } else if (receivedMessages.is_stopped()) {
            break;
        }
    }

    server->stopThreads();
    listener.request_stop();
    inputThread.request_stop();
    logger.log(Logger::LogLevel::Debug, "Requesting stop");
}


void runClient(const std::string &port, const std::string &address) {
    MessageQueue receivedMessages;
    const auto client = dynamic_cast<ClientSocket *>(appSocket.get());
    if (!client) {
        throw std::runtime_error("ClientSocket cast failed");
    }
    client->initSocket(port, address);

    std::jthread listener(&Socket::listenThread, std::ref(*client), std::ref(receivedMessages));
    std::stop_token stopToken = listener.get_stop_token();
    std::jthread inputThread(&userInputThread, std::ref(*client), std::ref(stopToken));

    while (!stopToken.stop_requested() && !shouldExit) {
        if (const auto message = receivedMessages.pop(stopToken)) {
            std::cout << *message << std::endl;
        } else if (receivedMessages.is_stopped()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    client->stopThreads();
    listener.request_stop();
    inputThread.request_stop();
    logger.log(Logger::LogLevel::Debug, "Requesting stop");
}
