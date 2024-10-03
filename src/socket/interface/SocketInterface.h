#pragma once
#include <string>
#include <utility>
#include <optional>

class  SocketInterface {
public:
    virtual ~SocketInterface() = default;
    virtual void initSocket(std::string port) = 0;

    virtual bool isValid() = 0;
    virtual void sendMessage(const std::wstring &message) = 0;
    virtual void sendMessageToClient(const std::wstring &message, const std::pair<unsigned int, std::string> &client) = 0;
    virtual std::wstring receiveMessage() = 0;
    virtual std::wstring receiveMessageFromClient(const std::pair<unsigned int, std::string> &client) = 0;
    virtual void connectTo(const std::string &hostname) = 0;
    virtual void bindAndListen() = 0;
    virtual std::optional<std::pair<unsigned int, std::string>> acceptConnectionListener() = 0;

};