#pragma once
#include <utility>

#include "Movable.h"
#include "Socket.h"
#include "Spinners.h"
#include "TextBox.h"
#include "TUI.h"
#include "UserInput.h"


class ChatApplication {
public:
    ChatApplication(std::unique_ptr<Socket> s, std::string p, std::string a = "")
        : m_socket(std::move(s)), m_port(std::move(p)), m_address(std::move(a)) {
    }

    void run(bool fun);

private:
    UserInput& input = UserInput::getInstance();
    std::unique_ptr<Socket> m_socket;
    std::string m_port;
    std::string m_address;
    std::unique_ptr<TUI> m_tui;
    std::shared_ptr<TextBox> m_chats;
    std::shared_ptr<TextBox> m_usersBoxText;
    std::shared_ptr<Movable> m_conUserMovable;
    MessageQueue m_receivedMessages;
    std::vector<std::pair<unsigned int, std::string> > m_clients;
    std::wstring m_userChat;

    void initializeTUI(bool fun);

    void setupLogger() const;

    void initializeSocket() const;

    void updateClientList();

    void handleUserInput();

    void updateUserChat(std::wstring_view newInput);

    static bool isExitCommand(std::wstring_view command);

    void sendAndClearUserChat();
};
