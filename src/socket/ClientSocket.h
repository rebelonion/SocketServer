#pragma once

#include "Socket.h"


class ClientSocket final : public Socket {
public:
    explicit ClientSocket(bool errorDetection) : Socket(false, errorDetection) {
    }

    std::vector<std::pair<unsigned int, std::string> > getClients() override {
        return {};
    }

    void initClientSocket(const std::string &port, std::string_view hostname);

    void sendMessage(const std::wstring &message, unsigned int client) override;

private:
    void connectToServer();

    [[nodiscard]] std::wstring receiveMessage() const;

    void listenThread(MessageQueue &receivedMessages) override;

    std::string m_hostname;
};
