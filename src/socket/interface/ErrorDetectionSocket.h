#pragma once

#include <vector>
#include <cstdint>
#include <memory>

#include "Globals.h"
#include "SocketInterface.h"

class ErrorDetectionSocket final : public SocketInterface {
public:
    explicit ErrorDetectionSocket(std::unique_ptr<SocketInterface> baseSocket);

    void initSocket(std::string port) override;

    bool isValid() override;

    void sendMessage(const std::wstring &message) override;

    void sendMessageToClient(const std::wstring &message, const std::pair<unsigned int, std::string> &client) override;

    std::wstring receiveMessage() override;

    std::wstring receiveMessageFromClient(const std::pair<unsigned int, std::string> &client) override;

    void connectTo(const std::string &hostname) override;

    void bindAndListen() override;

    std::optional<std::pair<unsigned int, std::string> > acceptConnectionListener() override;

private:
    std::unique_ptr<SocketInterface> m_baseSocket;
    static constexpr size_t MAX_MESSAGE_SIZE = 10;

    struct Packet {
        wchar_t type{};
        wchar_t number{};
        wchar_t total{};
        wchar_t message_length{};
        std::wstring message;
        uint8_t checksum{};
    };

    static uint8_t calculateChecksum(const Packet &packet);

    static std::vector<Packet> createPackets(const std::wstring &message);

    static std::wstring packetToWString(const Packet &packet);

    static Packet wstringToPacket(const std::wstring &data);

    void sendPacket(const Packet &packet,
                    const std::optional<std::pair<unsigned int, std::string> > &client = std::nullopt) const;

    [[nodiscard]] Packet receivePacket(
        const std::optional<std::pair<unsigned int, std::string> > &client = std::nullopt) const;
};
