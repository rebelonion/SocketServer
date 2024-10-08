#include "ChatApplication.h"
#include "Box.h"
#include "ClientSocket.h"
#include "Errors.h"
#include "Globals.h"
#include "ServerSocket.h"
#include "StringMod.h"
#include "Terminfo.h"
#include "UserInput.h"

void ChatApplication::run(const bool fun) {
    setupLogger();
    initializeTUI(fun);
    initializeSocket();

    const std::jthread listener(&Socket::listenThread, std::ref(*m_socket), std::ref(m_receivedMessages));
    const std::stop_token stopToken = listener.get_stop_token();

    while (!stopToken.stop_requested() && !shouldExit) {
        if (const auto message = m_receivedMessages.pop(stopToken)) {
            m_chats->addLine(*message);
        } else if (m_receivedMessages.is_stopped()) {
            break;
        }

        updateClientList();
        handleUserInput();
        m_tui->render();
    }
    logger.log(Logger::LogLevel::Debug, "Requesting stop");
}

void ChatApplication::initializeTUI(const bool fun) {
    auto size = TermInfo::getTerminalSize();
    const int maxChats = size.second - 4;
    if (size.first < 80 || size.second < 24) {
        size = {80, 24};
    }
    m_tui = std::make_unique<TUI>(size.first, size.second);
    m_chats = std::make_shared<TextBox>(1, 1, m_tui->getWidth() - 23, maxChats);
    m_chats->setDirection(TextBox::Direction::DOWN);

    const auto box = std::make_shared<Box>(0, 0, m_tui->getWidth(), m_tui->getHeight());
    box->addCrossbar(m_tui->getHeight() - 3);

    const auto usersBox = std::make_shared<Box>(m_tui->getWidth() - 22, 0, 22, m_tui->getHeight() - 2);
    usersBox->addCrossbar(2);

    m_usersBoxText = std::make_shared<TextBox>(m_tui->getWidth() - 21, 3, 20, m_tui->getHeight() - 6);
    m_usersBoxText->setDirection(TextBox::Direction::UP);

    m_conUserMovable = std::make_shared<Movable>(Spinners().randomSpinner(fun), 0.08, m_tui->getWidth() - 8, 1, 7,
                                                 1);

    m_tui->offerTItem(box);
    m_tui->offerTItem(usersBox);
    m_tui->offerTItem(m_usersBoxText);
    m_tui->offerTItem(m_conUserMovable);
    m_tui->offerTItem(m_chats);

    m_tui->draw(2, m_tui->getHeight() - 2, L"█");
    m_tui->draw(m_tui->getWidth() - 21, 1, L"Connections: ");
    m_tui->quickFullRender();
}

void ChatApplication::setupLogger() const {
    auto cLog = [this](const std::wstring &message) {
        m_chats->addLine(message);
    };
    logger.setCustomLogFunction(cLog);
}

void ChatApplication::initializeSocket() const {
    if (auto *server = dynamic_cast<ServerSocket *>(m_socket.get())) {
        server->initServerSocket(m_port);
        logger.log(Logger::LogLevel::Debug, "Port: " + m_port);
    } else if (auto *client = dynamic_cast<ClientSocket *>(m_socket.get())) {
        client->initClientSocket(m_port, m_address);
        logger.log(Logger::LogLevel::Debug, "Address: " + m_address);
    } else {
        throw SocketException("Invalid socket type");
    }
}

void ChatApplication::updateClientList() {
    if (const auto newClientList = m_socket->getClients(); m_clients != newClientList) {
        m_clients = newClientList;
        m_usersBoxText->clear();
        size_t position = 0;
        for (const auto &[id, ip]: m_clients) {
            const unsigned int color = StringMod::chooseColor(position);
            std::wstring users = StringMod::color(std::format(L"{}#{}", StringMod::toWString(ip), id), color);
            m_usersBoxText->addLine(users);
            ++position;
        }
    }
}

void ChatApplication::handleUserInput() {
    if (!input.inputAvailable()) {
        return;
    }

    const auto inputString = input.getInput(m_userChat);
    if (!inputString.has_value()) {
        return;
    }

    if (inputString.value() != m_userChat || m_userChat.empty()) {
        updateUserChat(inputString.value());
        return;
    }

    if (isExitCommand(m_userChat)) {
        shouldExit = true;
        return;
    }

    sendAndClearUserChat();
}

void ChatApplication::updateUserChat(const std::wstring_view newInput) {
    m_userChat = newInput;
    m_tui->draw(2, m_tui->getHeight() - 2, m_userChat + L"█" + std::wstring(2, L' '));
}

bool ChatApplication::isExitCommand(const std::wstring_view command) {
    return command == L"quit" || command == L"exit";
}

void ChatApplication::sendAndClearUserChat() {
    m_chats->addLine(m_userChat);
    m_socket->sendMessage(m_userChat, -1);
    m_userChat.clear();
    m_tui->draw(2, m_tui->getHeight() - 2, L"█" + std::wstring(m_tui->getWidth() - 4, L' '));
}
