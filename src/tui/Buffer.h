#pragma once

#include <string>
#include <vector>

class  Buffer {
public:
    Buffer(int width, int height);

    [[nodiscard]] std::wstring getLine(int y) const;

    void draw(int x, int y, const std::wstring& str);

    void clear();

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

private:
    int m_width;
    int m_height;
    std::vector<std::wstring> m_buffer;
};
