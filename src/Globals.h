#pragma once

#include <atomic>

#include "ConsoleOutput.h"
#include "util/Logger.h"

extern Logger logger;
extern std::atomic<bool> shouldExit;
extern ConsoleOutput cout;