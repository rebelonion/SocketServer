#include "ServerSocket.h"
#include "Globals.h"
#include "Logger.h"
#include "StringMod.h"

ServerSocket::~ServerSocket() = default;

void ServerSocket::initSocket(const std::string port) {
    Socket::initSocket(port);
    m_socket->bindAndListen();
}

void ServerSocket::acceptAndHandleConnection(const std::stop_token &stoken) {
    while (!stoken.stop_requested() && m_socket->isValid()) {
        if (auto newConnection = m_socket->acceptConnectionListener()) {
            std::lock_guard lock(m_clientsMutex);
            m_clients.push_back(newConnection.value());
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void ServerSocket::sendMessage(const std::string &message, const unsigned int client) {
    if (!m_socket->isValid()) {
        logger.log(Logger::LogLevel::Error, "Socket not initialized\n");
        return;
    }
    logger.log(Logger::LogLevel::Debug, "Sending message: " + message);
    std::string sender = "Server";
    unsigned int color = 31;
    if (client != -1) {
        std::lock_guard lock(m_clientsMutex);
        if (const auto it = std::ranges::find_if(m_clients, [client](const auto &clientSocket) {
            return clientSocket.first == client;
        }); it != m_clients.end()) {
            sender = it->second + "#" + std::to_string(it->first);
            const size_t position = std::distance(m_clients.begin(), it);
            color = StringMod::chooseColor(position);
        }
    }
    const std::string messageToSend = StringMod::color(sender, color) + ": " + message;
    std::lock_guard lock(m_clientsMutex);
    for (const auto &clientSocket: m_clients) {
        if (client != clientSocket.first) {
            m_socket->sendMessageToClient(messageToSend, clientSocket);
        }
    }
}

std::generator<std::string> ServerSocket::receiveMessages() {
    while (!m_connectionListenerThread.get_stop_token().stop_requested() && !shouldExit) {
        std::unique_lock lock(m_queueMutex);

        const bool messageAvailable = m_queueCV.wait_for(lock, std::chrono::milliseconds(100), [this] {
            return !m_messageQueue.empty() ||
                   m_connectionListenerThread.get_stop_token().stop_requested();
        });

        if (m_connectionListenerThread.get_stop_token().stop_requested()) {
            break;
        }

        if (messageAvailable) {
            while (!m_messageQueue.empty()) {
                co_yield m_messageQueue.front().first + m_messageQueue.front().second;
                m_messageQueue.pop();
            }
        }
    }
}


void ServerSocket::startThreads() {
    m_connectionListenerThread = std::jthread([this](const std::stop_token &stoken) {
        acceptAndHandleConnection(stoken);
    });
    m_messageReceiverThread = std::jthread([this](const std::stop_token &stoken) {
        messageReceiverThread(stoken);
    });
}

void ServerSocket::stopThreads() {
    m_connectionListenerThread.request_stop();
    m_messageReceiverThread.request_stop();
}

void ServerSocket::messageReceiverThread(const std::stop_token &stoken) {
    while (!stoken.stop_requested()) {
        std::vector<std::pair<unsigned int, std::string> > clients; {
            std::lock_guard lock(m_clientsMutex);
            clients = m_clients;
        }

        try {
            for (const auto &clientSocket: clients) {
                if (auto message = m_socket->receiveMessageFromClient(clientSocket); !message.empty()) {
                    std::lock_guard lock(m_queueMutex);
                    std::string client = clientSocket.second + "#" + std::to_string(clientSocket.first) + ": ";
                    const unsigned int pos = std::distance(clients.begin(), std::ranges::find(clients, clientSocket));
                    const unsigned int color = StringMod::chooseColor(pos);
                    m_messageQueue.emplace(StringMod::color(client, color), message);
                    m_queueCV.notify_one();
                    sendMessage(message, clientSocket.first);
                }
            }
        } catch (SocketClosedException &e) {
            logger.log(Logger::LogLevel::Error, e.what());
            std::lock_guard lock(m_clientsMutex);
            std::erase(m_clients, e.getClient());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
