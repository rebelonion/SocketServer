#include "Socket.h"

#include "interface/ErrorDetectionSocket.h"
#include "interface/LinuxSocket.h"
#include "interface/WindowsSocket.h"

Socket::Socket(const bool isServer, bool errorDetection) {
    m_isServer = isServer;
#ifdef _WIN32
    if (errorDetection) {
        m_socket = std::make_unique<ErrorDetectionSocket>(std::make_unique<WindowsSocket>());
    } else {
        m_socket = std::make_unique<WindowsSocket>();
    }
#elif __linux__
    if (errorDetection) {
        m_socket = std::make_unique<ErrorDetectionSocket>(std::make_unique<LinuxSocket>());
    } else {
        m_socket = std::make_unique<LinuxSocket>();
    }
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
