#include "ThreadSafeVector.h"

#include <cassert>

void ThreadSafeVector::add(const std::wstring &str) {
    std::lock_guard lock(mutex);
    vec.push_back(str);
}

void ThreadSafeVector::remove(const int index) {
    std::lock_guard lock(mutex);
    if (index >= 0 && index < vec.size()) {
        vec.erase(vec.begin() + index);
    }
}

void ThreadSafeVector::clear() {
    std::lock_guard lock(mutex);
    vec.clear();
}

std::wstring ThreadSafeVector::get(const int index) {
    std::lock_guard lock(mutex);
    if (index >= 0 && index < vec.size()) {
        return vec[index];
    }
    return L"";
}

size_t ThreadSafeVector::size() const {
    std::lock_guard lock(mutex);
    return vec.size();
}
