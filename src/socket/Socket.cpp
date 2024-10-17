#include "Socket.h"

#include "interface/ErrorDetectionSocket.h"
#include "interface/LinuxSocket.h"
#include "interface/WindowsSocket.h"

Socket::Socket(const bool isServer) {
    m_isServer = isServer;
#ifdef _WIN32
    //m_socket = std::make_unique<WindowsSocket>();
    m_socket = std::make_unique<ErrorDetectionSocket>(std::make_unique<WindowsSocket>());
#elif __linux__
    //m_socket = std::make_unique<LinuxSocket>();
    m_socket = std::make_unique<ErrorDetectionSocket>(std::make_unique<LinuxSocket>());
#else
    throw std::runtime_error("Unsupported platform");
#endif
}

Socket::~Socket() = default;

void Socket::initServerSocket(const std::string &port) {
    m_socket->initSocket(port);
}

bool Socket::isServer() const {
    return m_isServer;
}
