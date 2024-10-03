#include "WindowsSocket.h"

#include "Errors.h"
#include "StringMod.h"

#ifdef _WIN32
#include <vector>
#include "Globals.h"

WindowsSocket::WindowsSocket() {
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    initializeWinsock();
}

WindowsSocket::~WindowsSocket() {
    closesocket(m_socket);
    WSACleanup();
}

void WindowsSocket::initSocket(std::string port) {
    m_port = std::move(port);
    createSocket();
}

bool WindowsSocket::isValid() {
    return m_socket != INVALID_SOCKET;
}

void WindowsSocket::sendMessage(const std::wstring &message) {
    sendMessage(message, {m_socket, "host"});
}

void WindowsSocket::sendMessageToClient(const std::wstring &message,
                                        const std::pair<unsigned int, std::string> &client) {
    sendMessage(message, client);
}

std::wstring WindowsSocket::receiveMessage() {
    return receiveMessage({m_socket, "host"});
}

std::wstring WindowsSocket::receiveMessageFromClient(const std::pair<unsigned int, std::string> &client) {
    return receiveMessage(client);
}

void WindowsSocket::connectTo(const std::string &hostname) {
    addrinfo *result = nullptr;
    if (getaddrinfo(hostname.c_str(), m_port.c_str(), &hints, &result) != 0) {
        throw SocketException("getaddrinfo failed");
    }
    if (connect(m_socket, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(m_socket);
        throw SocketException("Error connecting to server");
    }
    freeaddrinfo(result);
}

void WindowsSocket::bindAndListen() {
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(static_cast<unsigned short>(std::stoi(m_port)));

    if (bind(m_socket, static_cast<sockaddr *>(static_cast<void *>(&serverAddr)), sizeof(serverAddr)) == SOCKET_ERROR) {
        throw SocketException("Error Binding Socket");
    }
    logger.log(Logger::LogLevel::Info, "Success Binding Socket");
    logger.log(Logger::LogLevel::Info, "Listening for socket connection");
    if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR) {
        throw SocketException("Error listening on socket");
    }
}

std::optional<std::pair<unsigned int, std::string> > WindowsSocket::acceptConnectionListener() {
    if (!m_isNonBlocking) {
        setNonBlocking(m_socket);
        m_isNonBlocking = true;
    }

    sockaddr_in clientAddr{};
    int clientAddrLen = sizeof(clientAddr);

    const auto clientSocket = accept(m_socket, static_cast<sockaddr *>(static_cast<void *>(&clientAddr)),
                                     &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        if (const int error = WSAGetLastError(); error == WSAEWOULDBLOCK) {
            return std::nullopt;
        } else {
            logger.log(Logger::LogLevel::Error, std::format(L"Error accepting new socket: {}", error));
            return std::nullopt;
        }
    }
    setNonBlocking(clientSocket);
    std::string clientIP(INET_ADDRSTRLEN, '\0');
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP.data(), INET_ADDRSTRLEN);
    logger.log(Logger::LogLevel::Info, L"Socket Accepted from: " + StringMod::toWString(clientIP));
    return std::make_pair(clientSocket, clientIP);
}

void WindowsSocket::initializeWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw SocketException("WSAStartup failed");
    }
}

void WindowsSocket::createSocket() {
    m_socket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
    if (m_socket == INVALID_SOCKET) {
        throw SocketException("Error creating socket");
    }
    logger.log(Logger::LogLevel::Info, "Socket created");
}

void WindowsSocket::sendMessage(const std::wstring &message, const std::pair<unsigned int, std::string> &client) {
    if (const std::string utf8Message = StringMod::toString(message); send(client.first, utf8Message.c_str(),
                                                                           static_cast<int>(utf8Message.length()),
                                                                           0) == SOCKET_ERROR) {
        logger.log(Logger::LogLevel::Error, std::format(L"Error sending message. Error code: {}", WSAGetLastError()));
        return;
    }
    logger.log(Logger::LogLevel::Debug, L"Sent message: " + message);
}

void WindowsSocket::setNonBlocking(const SOCKET socket) {
    u_long mode = 1;
    if (ioctlsocket(socket, FIONBIO, &mode) != 0) {
        throw SocketException("Failed to set socket to non-blocking mode");
    }
}

std::wstring WindowsSocket::receiveMessage(const std::pair<unsigned int, std::string> &client) {
    std::vector<char> buffer(4096);
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(client.first, &readSet);

    timeval timeout{};
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    const int ready = select(0, &readSet, nullptr, nullptr, &timeout);

    if (ready == SOCKET_ERROR) {
        throw SocketException(std::format("Select failed. Error code: {}", WSAGetLastError()));
    }

    if (ready > 0) {
        const int bytesReceived = recv(client.first, buffer.data(), static_cast<int>(buffer.size()) - 1, 0);
        if (bytesReceived == SOCKET_ERROR) {
            const int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                return L"";
            }
            if (error >= WSAECONNABORTED && error <= WSAESHUTDOWN) {
                throw SocketClosedException(client.second, client);
            }
            throw SocketException(std::format("Error reading from socket. Error code: {}", error));
        }
        if (bytesReceived == 0) {
            throw SocketClosedException(client.second, client);
        }
        buffer[bytesReceived] = '\0';
        const std::string utf8Message(buffer.data());
        return StringMod::toWString(utf8Message);
    }

    return L"";
}

#endif
