#include "Socket.h"

#include <utility>

#include "interface/WindowsSocket.h"

Socket::Socket(const bool isServer) {
    m_isServer = isServer;
#ifdef _WIN32
    m_socket = std::make_unique<WindowsSocket>();
#else
    throw std::runtime_error("Unsupported platform");
#endif
}

Socket::~Socket() = default;

void Socket::initSocket(std::string port) {
    m_socket->initSocket(std::move(port));
}

bool Socket::isServer() const {
    return m_isServer;
}

void Socket::listenThread(MessageQueue &receivedMessages) {
    startThreads();
    // ReSharper disable once CppLocalVariableMayBeConst (false positive)
    for (auto receiver = receiveMessages(); const auto &message: receiver) {
        receivedMessages.push(message);
    }
    stopThreads();
}
