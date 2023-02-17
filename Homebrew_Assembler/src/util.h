#pragma once

#include <iomanip>
#include <bitset>
#include <climits>

#define hex8 std::setfill('0') << std::setw(8) << std::hex << std::uppercase
#define hex4 std::setfill('0') << std::setw(4) << std::hex << std::uppercase
#define hex2 std::setfill('0') << std::setw(2) << std::hex << std::uppercase
#define dec std::dec