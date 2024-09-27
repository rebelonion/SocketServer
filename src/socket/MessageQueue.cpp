#include "MessageQueue.h"

void MessageQueue::push(const std::string &msg) {
    std::lock_guard lock(mtx);
    queue.push(msg);
    cv.notify_one();
}

std::optional<std::string> MessageQueue::pop(const std::stop_token& stopToken) {
    std::unique_lock lock(mtx);
    const auto _ = cv.wait_for(lock, std::chrono::milliseconds(100), [this, &stopToken] {
        return !queue.empty() || stopped || stopToken.stop_requested();
    });

    if (stopped && queue.empty()) {
        return std::nullopt;
    }

    if (stopToken.stop_requested()) {
        return std::nullopt;
    }

    if (!queue.empty()) {
        std::string msg = queue.front();
        queue.pop();
        return msg;
    }

    return std::nullopt;
}

void MessageQueue::stop() {
    std::lock_guard lock(mtx);
    stopped = true;
    cv.notify_all();
}

bool MessageQueue::is_stopped() const {
    std::lock_guard lock(mtx);
    return stopped;
}

