#pragma once

#include <optional>
#include "SocketInterface.h"
#ifdef __linux__
#include <netdb.h>

class LinuxSocket final : public SocketInterface {
public:
    LinuxSocket();

    ~LinuxSocket() override;

    void initSocket(std::string port) override;

    bool isValid() override;

    void sendMessage(const std::wstring &message) override;

    void sendMessageToClient(const std::wstring &message, const std::pair<unsigned int, std::string> &client) override;

    std::wstring receiveMessage() override;

    std::wstring receiveMessageFromClient(const std::pair<unsigned int, std::string> &client) override;

    void connectTo(const std::string &hostname) override;

    void bindAndListen() override;

    std::optional<std::pair<unsigned int, std::string> > acceptConnectionListener() override;

private:
    int m_socket;
    std::string m_port;
    bool m_isNonBlocking;
    addrinfo hints{};

    void createSocket();

    static void setNonBlocking(int socket);

    static void sendMessage(const std::wstring &message, const std::pair<unsigned int, std::string> &client);

    static std::wstring receiveMessage(const std::pair<unsigned int, std::string> &client);
};

#endif
