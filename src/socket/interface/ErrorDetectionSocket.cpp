#include "ErrorDetectionSocket.h"

ErrorDetectionSocket::ErrorDetectionSocket(std::unique_ptr<SocketInterface> baseSocket): m_baseSocket(
    std::move(baseSocket)) {
}

void ErrorDetectionSocket::initSocket(std::string port) {
    m_baseSocket->initSocket(std::move(port));
}

bool ErrorDetectionSocket::isValid() {
    return m_baseSocket->isValid();
}

void ErrorDetectionSocket::sendMessage(const std::wstring &message) {
    for (auto packets = createPackets(message); const auto &packet: packets) {
        sendPacket(packet);

        Packet response = receivePacket();
        while (response.type == PACKET_NAK) {
            response = receivePacket();
        }
    }
}

void ErrorDetectionSocket::sendMessageToClient(const std::wstring &message,
                                               const std::pair<unsigned int, std::string> &client) {
    auto packets = createPackets(message);
    logger.log(Logger::LogLevel::Debug, L"total packets: " + std::to_wstring(packets.size()));
    for (const auto &packet: packets) {
        sendPacket(packet, client);

        Packet response = receivePacket(client);
        while (response.type == PACKET_NAK) {
            response = receivePacket(client);
        }
    }
}

std::wstring ErrorDetectionSocket::receiveMessage() {
    std::wstring fullMessage;
    wchar_t totalPackets = 0;
    uint32_t receivedPackets = 0;

    do {
        Packet packet = receivePacket();

        if (packet.type == PACKET_NAK) {
            return L""; // No message available
        }

        if (calculateChecksum(packet) != packet.checksum) {
            sendPacket(Packet{PACKET_NAK, 0, 0, 0, L"", 0});
            continue;
        }

        sendPacket(Packet{PACKET_ACK, 0, 0, 0, L"", 0});

        logger.log(Logger::LogLevel::Debug, L"total packets: " + std::to_wstring(packet.total));

        receivedPackets++;
        fullMessage += packet.message;
        totalPackets = packet.total;
    } while (receivedPackets < static_cast<uint32_t>(totalPackets));

    return fullMessage;
}

std::wstring ErrorDetectionSocket::receiveMessageFromClient(const std::pair<unsigned int, std::string> &client) {
    std::wstring fullMessage;
    wchar_t totalPackets = 0;
    uint32_t receivedPackets = 0;

    do {
        Packet packet = receivePacket(client);

        if (packet.type == PACKET_NAK) {
            return L""; // No message available
        }

        if (calculateChecksum(packet) != packet.checksum) {
            sendPacket(Packet{PACKET_NAK, 0, 0, 0, L"", 0}, client);
            continue;
        }

        sendPacket(Packet{PACKET_ACK, 0, 0, 0, L"", 0}, client);

        receivedPackets++;
        fullMessage += packet.message;
        totalPackets = packet.total;
    } while (receivedPackets < static_cast<uint32_t>(totalPackets));

    return fullMessage;
}

void ErrorDetectionSocket::connectTo(const std::string &hostname) {
    m_baseSocket->connectTo(hostname);
}

void ErrorDetectionSocket::bindAndListen() {
    m_baseSocket->bindAndListen();
}

std::optional<std::pair<unsigned int, std::string> > ErrorDetectionSocket::acceptConnectionListener() {
    return m_baseSocket->acceptConnectionListener();
}

uint8_t ErrorDetectionSocket::calculateChecksum(const Packet &packet) {
    uint8_t sum = packet.type + packet.number + packet.total + packet.message_length;
    for (const wchar_t ch: packet.message) {
        sum += ch;
    }
    return sum;
}

std::vector<ErrorDetectionSocket::Packet> ErrorDetectionSocket::createPackets(const std::wstring &message) {
    std::vector<Packet> packets;
    const size_t totalPackets = (message.length() + MAX_MESSAGE_SIZE - 1) / MAX_MESSAGE_SIZE;

    for (size_t i = 0; i < totalPackets; ++i) {
        Packet packet;
        packet.type = PACKET_MSG;
        packet.number = static_cast<wchar_t>(i + 1);
        packet.total = static_cast<wchar_t>(totalPackets);

        const size_t start = i * MAX_MESSAGE_SIZE;
        const size_t end = std::min(start + MAX_MESSAGE_SIZE, message.length());
        packet.message = message.substr(start, end - start);
        packet.message_length = static_cast<wchar_t>(packet.message.length());

        packet.checksum = calculateChecksum(packet);
        packets.push_back(packet);
    }

    return packets;
}

std::wstring ErrorDetectionSocket::packetToWString(const Packet &packet) {
    std::wstring result;
    result.push_back(packet.type);
    result.push_back(packet.number);
    result.push_back(packet.total);
    result.push_back(packet.message_length);
    result += packet.message;
    result.push_back(packet.checksum);
    return result;
}

ErrorDetectionSocket::Packet ErrorDetectionSocket::wstringToPacket(const std::wstring &data) {
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

void ErrorDetectionSocket::sendPacket(const Packet &packet,
                                      const std::optional<std::pair<unsigned int, std::string> > &client) const {
    const std::wstring packetData = packetToWString(packet);
    if (client) {
        m_baseSocket->sendMessageToClient(packetData, *client);
    } else {
        m_baseSocket->sendMessage(packetData);
    }
}

ErrorDetectionSocket::Packet ErrorDetectionSocket::receivePacket(
    const std::optional<std::pair<unsigned int, std::string> > &client) const {
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
