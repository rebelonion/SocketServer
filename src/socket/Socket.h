#pragma once

#include <string>
#include <iostream>
#include <generator>
#include "MessageQueue.h"
#include "interface/SocketInterface.h"

class Socket {
public:
    explicit Socket(bool isServer);

    virtual ~Socket();

    virtual std::vector<std::pair<unsigned int, std::string> > getClients() = 0;

    virtual void initSocket(std::string port);

    virtual void sendMessage(const std::wstring &message, unsigned int client) = 0;

    void listenThread(MessageQueue &receivedMessages);

    [[nodiscard]] bool isServer() const;

    virtual void startThreads() = 0;

    virtual void stopThreads() = 0;

private:
    bool m_isServer = false;

protected:
    virtual std::generator<std::wstring> receiveMessages() = 0;

    std::unique_ptr<SocketInterface> m_socket;
};
