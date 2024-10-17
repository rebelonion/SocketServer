#pragma once

#include <atomic>

#include "ConsoleOutput.h"
#include "util/Logger.h"

#define PACKET_MSG 0xFD
#define PACKET_ACK 0xFE
#define PACKET_NAK 0xFF

extern Logger logger;
extern std::atomic<bool> shouldExit;
extern ConsoleOutput cout;