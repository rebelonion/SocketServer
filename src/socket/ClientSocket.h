#pragma once

#include "Socket.h"


class ClientSocket final : public Socket {
public:
    explicit ClientSocket() : Socket(false) {
    }

    void initSocket(const std::string &port, const std::string &hostname);

    void sendMessage(const std::wstring &message, unsigned int client) override;


    void startThreads() override;

    void stopThreads() override;

private:
    void connectToServer();

    [[nodiscard]] std::wstring receiveMessage() const;

    std::generator<std::wstring> receiveMessages() override;

    std::string m_hostname;
};
