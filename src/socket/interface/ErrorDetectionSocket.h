#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <numeric>

#include "Globals.h"
#include "SocketInterface.h"

class ErrorDetectionSocket : public SocketInterface {
private:
    std::unique_ptr<SocketInterface> m_baseSocket;
    static constexpr size_t MAX_MESSAGE_SIZE = 10;

    struct Packet {
        wchar_t type;
        wchar_t number;
        wchar_t total;
        wchar_t message_length;
        std::wstring message;
        wchar_t checksum;
    };

    static wchar_t calculateChecksum(const Packet& packet) {
        wchar_t sum = packet.type + packet.number + packet.total + packet.message_length;
        for (wchar_t ch : packet.message) {
            sum += ch;
        }
        return sum;
    }

    std::vector<Packet> createPackets(const std::wstring& message) {
        std::vector<Packet> packets;
        size_t totalPackets = (message.length() + MAX_MESSAGE_SIZE - 1) / MAX_MESSAGE_SIZE;

        for (size_t i = 0; i < totalPackets; ++i) {
            Packet packet;
            packet.type = PACKET_MSG;
            packet.number = static_cast<wchar_t>(i + 1);
            packet.total = static_cast<wchar_t>(totalPackets);

            size_t start = i * MAX_MESSAGE_SIZE;
            size_t end = std::min(start + MAX_MESSAGE_SIZE, message.length());
            packet.message = message.substr(start, end - start);
            packet.message_length = static_cast<wchar_t>(packet.message.length());

            packet.checksum = calculateChecksum(packet);
            packets.push_back(packet);
        }

        return packets;
    }

    std::wstring packetToWString(const Packet& packet) {
        std::wstring result;
        result.push_back(packet.type);
        result.push_back(packet.number);
        result.push_back(packet.total);
        result.push_back(packet.message_length);
        result += packet.message;
        result.push_back(packet.checksum);
        return result;
    }

    Packet wstringToPacket(const std::wstring& data) {
        if (data.length() < 5) {
            if (data.length() == 1 && (data[0] == PACKET_ACK || data[0] == PACKET_NAK)) {
                return Packet{data[0], 0, 0, 0, L"", 0};
            }
            throw std::runtime_error("Invalid packet data");
        }

        Packet packet;
        packet.type = data[0];
        packet.number = data[1];
        packet.total = data[2];
        packet.message_length = data[3];
        packet.message = data.substr(4, packet.message_length);
        packet.checksum = data[4 + packet.message_length];

        return packet;
    }

    void sendPacket(const Packet& packet, const std::optional<std::pair<unsigned int, std::string>>& client = std::nullopt) {
        std::wstring packetData = packetToWString(packet);
        if (client) {
            m_baseSocket->sendMessageToClient(packetData, *client);
        } else {
            m_baseSocket->sendMessage(packetData);
        }
    }

    Packet receivePacket(const std::optional<std::pair<unsigned int, std::string>>& client = std::nullopt) {
        std::wstring packetData;
        if (client) {
            packetData = m_baseSocket->receiveMessageFromClient(*client);
        } else {
            packetData = m_baseSocket->receiveMessage();
        }

        if (packetData.empty()) {
            return Packet{PACKET_NAK, 0, 0, 0, L"", 0};
        }

        return wstringToPacket(packetData);
    }

public:
    ErrorDetectionSocket(std::unique_ptr<SocketInterface> baseSocket)
        : m_baseSocket(std::move(baseSocket)) {}

    void initSocket(std::string port) override {
        m_baseSocket->initSocket(std::move(port));
    }

    bool isValid() override {
        return m_baseSocket->isValid();
    }

    void sendMessage(const std::wstring& message) override {
        auto packets = createPackets(message);
        for (const auto& packet : packets) {
            sendPacket(packet);

            Packet response = receivePacket();
            while (response.type == PACKET_NAK) {
                sendPacket(packet);
                response = receivePacket();
            }
        }
    }

    void sendMessageToClient(const std::wstring& message, const std::pair<unsigned int, std::string>& client) override {
        auto packets = createPackets(message);
        for (const auto& packet : packets) {
            sendPacket(packet, client);

            Packet response = receivePacket(client);
            while (response.type == PACKET_NAK) {
                sendPacket(packet, client);
                response = receivePacket(client);
            }
        }
    }

    std::wstring receiveMessage() override {
        std::wstring fullMessage;
        wchar_t totalPackets = 0;

        do {
            Packet packet = receivePacket();

            if (packet.type == PACKET_NAK) {
                return L"";  // No message available
            }

            if (calculateChecksum(packet) != packet.checksum) {
                sendPacket(Packet{PACKET_NAK, 0, 0, 0, L"", 0});
                continue;
            }

            sendPacket(Packet{PACKET_ACK, 0, 0, 0, L"", 0});

            fullMessage += packet.message;
            totalPackets = packet.total;

        } while (fullMessage.length() < totalPackets * MAX_MESSAGE_SIZE);

        return fullMessage;
    }

    std::wstring receiveMessageFromClient(const std::pair<unsigned int, std::string>& client) override {
        std::wstring fullMessage;
        wchar_t totalPackets = 0;

        do {
            Packet packet = receivePacket(client);

            if (packet.type == PACKET_NAK) {
                return L"";  // No message available
            }

            if (calculateChecksum(packet) != packet.checksum) {
                sendPacket(Packet{PACKET_NAK, 0, 0, 0, L"", 0}, client);
                continue;
            }

            sendPacket(Packet{PACKET_ACK, 0, 0, 0, L"", 0}, client);

            fullMessage += packet.message;
            totalPackets = packet.total;

        } while (fullMessage.length() < totalPackets * MAX_MESSAGE_SIZE);

        return fullMessage;
    }

    void connectTo(const std::string& hostname) override {
        m_baseSocket->connectTo(hostname);
    }

    void bindAndListen() override {
        m_baseSocket->bindAndListen();
    }

    std::optional<std::pair<unsigned int, std::string>> acceptConnectionListener() override {
        return m_baseSocket->acceptConnectionListener();
    }
};