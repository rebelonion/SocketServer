#include "LinuxSocket.h"

#include <format>
#include <cstring>
#include <string>
#include <optional>
#include <stdexcept>

#ifdef __linux__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <vector>

#include "Globals.h"
#include "SocketInterface.h"
#include "Logger.h"

LinuxSocket::LinuxSocket() : m_socket(-1), m_isNonBlocking(false) {
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
}

LinuxSocket::~LinuxSocket() {
    if (m_socket != -1) {
        close(m_socket);
    }
}

void LinuxSocket::initSocket(std::string port) {
    m_port = std::move(port);
    createSocket();
}

bool LinuxSocket::isValid() {
    return m_socket != -1;
}

void LinuxSocket::sendMessage(const std::string &message) {
    sendMessage(message, {m_socket, "host"});
}

void LinuxSocket::sendMessageToClient(const std::string &message, const std::pair<unsigned int, std::string> &client) {
    sendMessage(message, client);
}

std::string LinuxSocket::receiveMessage() {
    return receiveMessage({m_socket, "host"});
}

std::string LinuxSocket::receiveMessageFromClient(const std::pair<unsigned int, std::string> &client) {
    return receiveMessage(client);
}

void LinuxSocket::connectTo(const std::string hostname) {
    addrinfo *result = nullptr;
    if (getaddrinfo(hostname.c_str(), m_port.c_str(), &hints, &result) != 0) {
        throw std::runtime_error("getaddrinfo failed");
    }
    if (connect(m_socket, result->ai_addr, static_cast<int>(result->ai_addrlen)) == -1) {
        freeaddrinfo(result);
        close(m_socket);
        throw std::runtime_error("Error connecting to server");
    }
    freeaddrinfo(result);
}

void LinuxSocket::bindAndListen() {
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(std::stoi(m_port));

    if (bind(m_socket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) == -1) {
        throw std::runtime_error("Error Binding Socket");
    }
    logger.log(Logger::LogLevel::Info, "Success Binding Socket");
    logger.log(Logger::LogLevel::Info, "Listening for socket connection");
    if (listen(m_socket, SOMAXCONN) == -1) {
        throw std::runtime_error("Error listening on socket");
    }
}

std::optional<std::pair<unsigned int, std::string> > LinuxSocket::acceptConnectionListener() {
    if (!m_isNonBlocking) {
        setNonBlocking(m_socket);
        m_isNonBlocking = true;
    }

    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);

    const int clientSocket = accept(m_socket, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrLen);
    if (clientSocket == -1) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return std::nullopt;
        }
        logger.log(Logger::LogLevel::Error, "Error accepting new socket: " + std::to_string(errno));
        return std::nullopt;
    }
    setNonBlocking(clientSocket);
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    logger.log(Logger::LogLevel::Info, std::format("Socket Accepted from {}\n", clientIP));
    return std::make_pair(clientSocket, clientIP);
}

void LinuxSocket::createSocket() {
    m_socket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
    if (m_socket == -1) {
        throw std::runtime_error("Error creating socket");
    }
    logger.log(Logger::LogLevel::Info, "Socket created");
}

void LinuxSocket::setNonBlocking(const int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("Failed to get socket flags");
    }
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw std::runtime_error("Failed to set socket to non-blocking mode");
    }
}

void LinuxSocket::sendMessage(const std::string &message, const std::pair<unsigned int, std::string> &client) {
    if (const int maxFd = static_cast<int>(std::min(static_cast<unsigned int>(INT_MAX), client.first)); send(maxFd, message.c_str(), message.length(), 0) == -1) {
        throw std::runtime_error("Error sending message");
    }
}

std::string LinuxSocket::receiveMessage(const std::pair<unsigned int, std::string> &client) {
    std::vector<char> buffer(4096);
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(client.first, &readSet);

    timeval timeout{};
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    const int maxFd = static_cast<int>(std::min(static_cast<unsigned int>(INT_MAX), client.first + 1));
    const int ready = select(maxFd, &readSet, nullptr, nullptr, &timeout);

    if (ready == -1) {
        throw std::runtime_error("Select failed. Error code: " + std::to_string(errno));
    }

    if (ready > 0) {
        const ssize_t bytesReceived = recv(maxFd - 1, buffer.data(), static_cast<int>(buffer.size()) - 1, 0);
        if (bytesReceived == -1) {
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                if (errno == ECONNRESET) {
                    throw SocketClosedException(client.second, client);
                }
                throw std::runtime_error("Error reading from socket. Error code: " + std::to_string(errno));
            }
            return "";
        }
        if (bytesReceived == 0) {
            return "";
        }
        buffer[bytesReceived] = '\0';
        return {buffer.data()};
    }

    return "";
}

#endif
