#pragma once
#include <stdint.h>

namespace GBCEmu {
inline int getBit(uint64_t n, uint64_t digit)
{
    return n & (1 << digit) ? 1 : 0;
}

inline void setBit(uint64_t& n, uint64_t digit, bool val)
{
    if (val) {
        n |= (1 << digit);
    } else {
        n &= ~(1 << digit);
    }
}

inline bool between(uint64_t a, uint64_t b, uint64_t c)
{
    return (a <= b) && (b <= c);
}

void delay(uint32_t ms);
}