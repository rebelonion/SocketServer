#pragma once
#include <format>
#include "SocketInterface.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

class WindowsSocket final : public SocketInterface {
public:
    WindowsSocket();

    ~WindowsSocket() override;

    void initSocket(std::string port) override;

    bool isValid() override;

    void sendMessage(const std::wstring &message) override;

    void sendMessageToClient(const std::wstring &message, const std::pair<unsigned int, std::string> &client) override;

    std::wstring receiveMessage() override;

    std::wstring receiveMessageFromClient(const std::pair<unsigned int, std::string> &client) override;

    void connectTo(std::string hostname) override;

    void bindAndListen() override;

    std::optional<std::pair<unsigned int, std::string>> acceptConnectionListener() override;

private:
    SOCKET m_socket = INVALID_SOCKET;
    std::string m_port{};
    addrinfo hints{};
    bool m_isNonBlocking = false;


    static void initializeWinsock();

    void createSocket();

    static void sendMessage(const std::wstring &message, const std::pair<unsigned int, std::string> &client);

    static std::wstring receiveMessage(const std::pair<unsigned int, std::string> &client);

    static void setNonBlocking(SOCKET socket);
};

#endif