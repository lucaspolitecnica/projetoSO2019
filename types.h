#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <array>

#define ASCII2HEXHALF(x) ((BYTE)((x < 0x3A) ? x - 0x30 : x - 0x37))
#define ASCII2HEX(hi,lo) ((BYTE)((ASCII2HEXHALF(hi) << 4) | ASCII2HEXHALF(lo)))

using BYTE = uint8_t;
using WORD = uint16_t;

constexpr size_t blockSize = 16;
using Block = std::array<BYTE, blockSize>;

#endif // TYPES_H
