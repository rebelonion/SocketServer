#include "ClientSocket.h"

#include "Globals.h"
#include "Logger.h"

void ClientSocket::initSocket(const std::string& port, const std::string &hostname) {
    Socket::initSocket(port);
    m_hostname = hostname;
    connectToServer();
}

void ClientSocket::connectToServer() {
    m_socket->connectTo(m_hostname);
    logger.log(Logger::LogLevel::Info, std::format("Connected to {}", m_hostname));
}

void ClientSocket::sendMessage(const std::wstring &message, unsigned int client) {
    if (!m_socket->isValid()) {
        logger.log(Logger::LogLevel::Error, "Socket not initialized");
        return;
    }
    logger.log(Logger::LogLevel::Debug, L"Sending message: " + message);
    //don't need to check if client is valid, since it's a client
    m_socket->sendMessage(message);
}

std::wstring ClientSocket::receiveMessage() const {
    if (!m_socket->isValid()) {
        throw std::runtime_error("Socket not initialized");
    }
    try {
        return m_socket->receiveMessage();
    } catch (SocketClosedException &_) {
        logger.log(Logger::LogLevel::Error, "Socket closed");
        shouldExit = true;
        return L"";
    }
}

std::generator<std::wstring> ClientSocket::receiveMessages() {
    while (m_socket->isValid() && !shouldExit) {
        if (auto message = receiveMessage(); !message.empty()) {
            co_yield message;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void ClientSocket::startThreads() {}
void ClientSocket::stopThreads() {}