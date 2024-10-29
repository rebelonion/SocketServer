#pragma once

#include <atomic>

#include "ConsoleOutput.h"
#include "util/Logger.h"

#define PACKET_ACK 0x02
#define PACKET_NAK 0x03
#define PACKET_MSG 0x01

extern Logger logger;
extern std::atomic<bool> shouldExit;
extern ConsoleOutput cout;