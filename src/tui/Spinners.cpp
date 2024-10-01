#include "Spinners.h"

std::vector<std::vector<std::wstring>> Spinners::randomSpinner(const bool includeEmoji) const {
    std::vector<std::vector<std::vector<std::wstring> > > spinners = {};
    if (includeEmoji) {
        spinners = {
            bounceSpinner, dotSpinner, arrowSpinner, boxSpinner, clockSpinner, moonSpinner, circularDotsSpinner,
            lineSpinner, starSpinner, bouncingBallSpinner, triangularSpinner, dnaSpinner, raindropSpinner,
            heartbeatSpinner, diceSpinner, jugglingSpinner, binaryStreamSpinner, weatherSpinner, fireworksSpinner,
            pacmanSpinner, yinYangSpinner
        };
    } else {
        spinners = {
            bounceSpinner, dotSpinner, arrowSpinner, boxSpinner, lineSpinner, starSpinner, bouncingBallSpinner,
            triangularSpinner, diceSpinner, raindropSpinner, binaryStreamSpinner, pacmanSpinner, yinYangSpinner
        };
    }
    std::default_random_engine generator(std::random_device{}());
    std::uniform_int_distribution<> distribution(0, static_cast<int>(spinners.size()) - 1);
    return spinners[distribution(generator)];
}
