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


    virtual void sendMessage(const std::wstring &message, unsigned int client) = 0;

    virtual void listenThread(MessageQueue &receivedMessages) = 0;

    [[nodiscard]] bool isServer() const;

private:
    bool m_isServer = false;

protected:

    virtual void initServerSocket(const std::string &port);

    std::unique_ptr<SocketInterface> m_socket;
};