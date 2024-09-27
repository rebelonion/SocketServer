#pragma once

#include <thread>
#include "Socket.h"

class ServerSocket final : public Socket {
public:
    explicit ServerSocket() : Socket(true) {}

    ~ServerSocket() override;

    void initSocket(std::string port) override;

    void sendMessage(const std::string &message, unsigned int client) override;

    void startThreads() override;

    void stopThreads() override;

private:
    void acceptAndHandleConnection(const std::stop_token& stoken);

    void messageReceiverThread(const std::stop_token& stoken);

    std::generator<std::string> receiveMessages() override;

    std::vector<std::pair<unsigned int, std::string>> m_clients;
    std::mutex m_clientsMutex;
    std::atomic<bool> m_isRunning{true};
    std::mutex m_queueMutex;
    std::condition_variable m_queueCV;
    std::queue<std::pair<std::string, std::string>> m_messageQueue;

    std::jthread m_connectionListenerThread;
    std::jthread m_messageReceiverThread;
};
