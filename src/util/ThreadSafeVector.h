#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <vector>

class ThreadSafeVector {
    std::vector<std::wstring> vec;
    mutable std::mutex mutex;

public:
    void add(const std::wstring& str);

    void remove(int index);

    std::wstring get (int index);


    size_t size() const;
};
