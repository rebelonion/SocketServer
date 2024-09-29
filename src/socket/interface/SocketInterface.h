#pragma once
#include <string>
#include <utility>
#include <optional>
#include <stdexcept>

class  SocketInterface {
public:
    virtual ~SocketInterface() = default;
    virtual void initSocket(std::string port) = 0;

    virtual bool isValid() = 0;
    virtual void sendMessage(const std::wstring &message) = 0;
    virtual void sendMessageToClient(const std::wstring &message, const std::pair<unsigned int, std::string> &client) = 0;
    virtual std::wstring receiveMessage() = 0;
    virtual std::wstring receiveMessageFromClient(const std::pair<unsigned int, std::string> &client) = 0;
    virtual void connectTo(std::string hostname) = 0;
    virtual void bindAndListen() = 0;
    virtual std::optional<std::pair<unsigned int, std::string>> acceptConnectionListener() = 0;

};


class SocketClosedException final : public std::runtime_error {
public:
    explicit SocketClosedException(const std::string& host, const std::pair<unsigned int, std::string> &client) : std::runtime_error("Socket closed for " + host) {
        m_client = client;
    }

    [[nodiscard]] std::pair<unsigned int, std::string> getClient() const {
        return m_client;
    }

private:
    std::pair<unsigned int, std::string> m_client;
};