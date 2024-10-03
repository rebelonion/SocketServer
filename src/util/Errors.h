#pragma once
#include <stdexcept>

class SocketException final : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class ParseError final : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class StartupError final : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class SocketClosedException final : public std::runtime_error {
public:
    explicit SocketClosedException(const std::string& host, const std::pair<unsigned int, std::string> &client) : std::runtime_error(
        "Socket closed for " + host), m_client(client) {
    }

    [[nodiscard]] std::pair<unsigned int, std::string> getClient() const {
        return m_client;
    }

private:
    std::pair<unsigned int, std::string> m_client;
};