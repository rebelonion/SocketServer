#pragma once
#include <random>
#include <string>
#include <vector>

class Spinners {  // this exists just for fun and I was bored ¯\_(ツ)_/¯
public:
    const std::vector<std::vector<std::wstring> > bounceSpinner = {
        {L"⠋"}, {L"⠙"}, {L"⠚"}, {L"⠞"}, {L"⠖"}, {L"⠦"}, {L"⠴"}, {L"⠲"}, {L"⠳"}, {L"⠓"}
    };
    const std::vector<std::vector<std::wstring> > dotSpinner = {
        {L"⠋"}, {L"⠙"}, {L"⠹"}, {L"⠸"}, {L"⠼"}, {L"⠴"}, {L"⠦"}, {L"⠧"}, {L"⠇"}, {L"⠏"}
    };
    const std::vector<std::vector<std::wstring> > arrowSpinner = {
        {L"←"}, {L"↖"}, {L"↑"}, {L"↗"}, {L"→"}, {L"↘"}, {L"↓"}, {L"↙"}
    };
    const std::vector<std::vector<std::wstring> > boxSpinner = {
        {L"▖"}, {L"▘"}, {L"▝"}, {L"▗"}
    };
    const std::vector<std::vector<std::wstring> > clockSpinner = {
        {L"🕐"}, {L"🕑"}, {L"🕒"}, {L"🕓"}, {L"🕔"}, {L"🕕"}, {L"🕖"}, {L"🕗"}, {L"🕘"}, {L"🕙"}, {L"🕚"}, {L"🕛"}
    };
    const std::vector<std::vector<std::wstring> > moonSpinner = {
        {L"🌑"}, {L"🌒"}, {L"🌓"}, {L"🌔"}, {L"🌕"}, {L"🌖"}, {L"🌗"}, {L"🌘"}
    };
    const std::vector<std::vector<std::wstring> > circularDotsSpinner = {
        {L"◜ "}, {L"◠ "}, {L"◝ "}, {L"◞ "}, {L"◡ "}, {L"◟ "}
    };
    const std::vector<std::vector<std::wstring> > lineSpinner = {
        {L"│"}, {L"/"}, {L"─"}, {L"\\"}
    };
    const std::vector<std::vector<std::wstring> > starSpinner = {
        {L"✶"}, {L"✸"}, {L"✹"}, {L"✺"}, {L"✹"}, {L"✷"}
    };
    const std::vector<std::vector<std::wstring> > bouncingBallSpinner = {
        {L"⠁"}, {L"⠂"}, {L"⠄"}, {L"⠂"}
    };
    const std::vector<std::vector<std::wstring> > triangularSpinner = {
        {L"◢"}, {L"◣"}, {L"◤"}, {L"◥"}
    };
    const std::vector<std::vector<std::wstring> > dnaSpinner = {
        {L"╔══╗"}, {L"║><║"}, {L"║><║"}, {L"║><║"},
        {L"╚══╝"}, {L"║><║"}, {L"║><║"}, {L"║><║"}
    };
    const std::vector<std::vector<std::wstring> > raindropSpinner = {
        {L"｡･"}, {L"･｡･"}, {L"｡･｡･"}, {L"･｡･｡"}, {L"｡･｡"}, {L"･｡"}
    };
    const std::vector<std::vector<std::wstring> > heartbeatSpinner = {
        {L"💗"}, {L"💓"}, {L"💗"}, {L"💓"}, {L"💗"}, {L"💓"}
    };
    const std::vector<std::vector<std::wstring> > diceSpinner = {
        {L"⚀"}, {L"⚁"}, {L"⚂"}, {L"⚃"}, {L"⚄"}, {L"⚅"}
    };
    const std::vector<std::vector<std::wstring> > jugglingSpinner = {
        {L"🔴  "}, {L" 🟡 "}, {L"  🔵"}, {L" 🟡 "}
    };
    const std::vector<std::vector<std::wstring> > binaryStreamSpinner = {
        {L"10101"}, {L"01010"}, {L"101010"}, {L"010101"},
        {L"1010101"}, {L"0101010"}
    };
    const std::vector<std::vector<std::wstring> > weatherSpinner = {
        {L"☀️"}, {L"🌤️"}, {L"⛅"}, {L"🌥️"}, {L"☁️"}, {L"🌧️"}, {L"⛈️"}, {L"🌨️"}, {L"❄️"}
    };
    const std::vector<std::vector<std::wstring> > fireworksSpinner = {
        {L"."}, {L"｡"}, {L"✺"}, {L"✹"}, {L"✸"}, {L"✷"}, {L"✶"}, {L"✵"}, {L"✳"}, {L" "}
    };
    const std::vector<std::vector<std::wstring> > pacmanSpinner = {
        {L"ᗧ···"}, {L"·ᗧ··"}, {L"··ᗧ·"}, {L"···ᗧ"},
        {L"··•ᗧ"}, {L"·•·ᗧ"}, {L"•··ᗧ"}, {L"···ᗧ"}
    };
    const std::vector<std::vector<std::wstring> > yinYangSpinner = {
        {L"◐"}, {L"◓"}, {L"◑"}, {L"◒"}
    };

    [[nodiscard]] std::vector<std::vector<std::wstring> > randomSpinner(bool includeEmoji = true) const;
};
