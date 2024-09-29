#include <iostream>
#include <utility>
#include <vector>
#include <memory>
#include "Logger.h"
#include "Socket.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "Globals.h"
#include "Terminfo.h"
#include "TextBox.h"
#include "TUI.h"
#include "UserInput.h"
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#endif

void runServer(const std::string& port);

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

void runChatApplication(Socket* socket, const std::string& port, const std::string& address = "") {
    std::wstring userChat;
    auto size = TermInfo::getTerminalSize();
    const int maxChats = size.second - 4;
    if (size.first < 80 || size.second < 24) {
        size = {80, 24};
    }
    TUI tui(size.first, size.second);
    const auto chats = std::make_shared<TextBox>(1, 1, tui.getWidth() - 2, maxChats);
    auto cLog = [chats] (const std::wstring& message) {
        chats->addLine(message);
    };

    logger.setCustomLogFunction(cLog);
    chats->setDirection(TextBox::Direction::DOWN);
    tui.offerTItem(chats);
    tui.draw(2, tui.getHeight() - 2, L"█");

    MessageQueue receivedMessages;

    if (auto* server = dynamic_cast<ServerSocket*>(socket)) {
        server->initSocket(port);
        logger.log(Logger::LogLevel::Debug, "Port: " + port);
    } else if (auto* client = dynamic_cast<ClientSocket*>(socket)) {
        client->initSocket(port, address);
        logger.log(Logger::LogLevel::Debug, "Address: " + address);
    } else {
        throw std::runtime_error("Invalid socket type");
    }



    std::jthread listener(&Socket::listenThread, std::ref(*socket), std::ref(receivedMessages));
    std::stop_token stopToken = listener.get_stop_token();

    while (!stopToken.stop_requested() && !shouldExit) {
        if (const auto message = receivedMessages.pop(stopToken)) {
            chats->addLine(*message);
        } else if (receivedMessages.is_stopped()) {
            break;
        }

        if (UserInput::inputAvailable()) {
            if (const auto input = UserInput::getInput(userChat); input.has_value()) {
                if (input.value() == userChat && !userChat.empty()) {
                    if (userChat == L"quit" || userChat == L"exit") {
                        shouldExit = true;
                        break;
                    }
                    chats->addLine(userChat);
                    socket->sendMessage(userChat, -1);
                    userChat.clear();
                    tui.draw(2, tui.getHeight() - 2, L"█" + std::wstring(tui.getWidth() - 4, L' '));
                } else {
                    const size_t spacesToAdd = (userChat.size() > input.value().size())
                                                   ? userChat.size() - input.value().size()
                                                   : 0;
                    userChat = input.value();
                    tui.draw(2, tui.getHeight() - 2, userChat + L"█" + std::wstring(spacesToAdd, L' '));
                }
            }
        }
        tui.render();
    }

    socket->stopThreads();
    listener.request_stop();
    logger.log(Logger::LogLevel::Debug, "Requesting stop");
}

void runServer(const std::string& port) {
    const auto server = dynamic_cast<ServerSocket*>(appSocket.get());
    if (!server) {
        throw std::runtime_error("ServerSocket cast failed");
    }
    runChatApplication(server, port);
}

void runClient(const std::string& port, const std::string& address) {
    const auto client = dynamic_cast<ClientSocket*>(appSocket.get());
    if (!client) {
        throw std::runtime_error("ClientSocket cast failed");
    }
    runChatApplication(client, port, address);
}