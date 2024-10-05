#pragma once

#include <string>
#include <vector>

class Buffer {
public:
    Buffer(int width, int height);

    [[nodiscard]] std::wstring getLine(int y) const;

    void draw(int x, int y, std::wstring_view str);

    void clear();

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

private:
    int m_width;
    int m_height;
    std::vector<std::vector<std::wstring> > m_buffer;

    [[nodiscard]] bool isValidPosition(int y) const;

    static void handleEscapeSequence(std::wstring &current_cell, const std::wstring_view &str, size_t &i,
                                     bool &in_escape_sequence);

    void handleViewableCharacter(std::wstring &current_cell, const std::wstring_view &str, size_t &i, int &pos);

    static void handleNonViewableCharacter(std::wstring &current_cell, const std::wstring_view &str, size_t &i);

    void finalizeCell(std::wstring &current_cell, int &pos, const std::wstring_view &str, size_t i);
};
