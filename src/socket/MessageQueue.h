#pragma once

#include <mutex>
#include <string>
#include <condition_variable>
#include <queue>

class MessageQueue {
    std::queue<std::string> queue;
    mutable std::mutex mtx;
    std::condition_variable cv;
    bool stopped = false;

public:
    void push(const std::string &msg);

    std::optional<std::string> pop(const std::stop_token& stopToken);

    void stop();

    bool is_stopped() const;
};
