
#include "StringMod.h"

std::string StringMod::color(const std::string &text, const unsigned int color) {
    return "\033[" + std::to_string(color) + "m" + text + "\033[0m";
}

unsigned int StringMod::chooseColor(const unsigned int pos) {
    return 32 + (pos % 5);
}