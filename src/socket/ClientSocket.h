#pragma once

#include "Socket.h"


class ClientSocket final : public Socket {
public:
    explicit ClientSocket() : Socket(false) {
    }

    void initSocket(const std::string &port, const std::string &hostname);

    void sendMessage(const std::string &message, unsigned int client) override;


    void startThreads() override;

    void stopThreads() override;

private:
    void connectToServer();

    [[nodiscard]] std::string receiveMessage() const;

    std::generator<std::string> receiveMessages() override;

    std::string m_hostname;
};
