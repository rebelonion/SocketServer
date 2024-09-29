#pragma once

#include <mutex>
#include <string>
#include <condition_variable>
#include <queue>

class MessageQueue {
    std::queue<std::wstring> queue;
    mutable std::mutex mtx;
    std::condition_variable cv;
    bool stopped = false;

public:
    void push(const std::wstring &msg);

    std::optional<std::wstring> pop(const std::stop_token& stopToken);

    void stop();

    bool is_stopped() const;
};
