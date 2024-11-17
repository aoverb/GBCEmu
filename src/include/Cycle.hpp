#pragma once
#include "Common.hpp"

namespace GBCEmu {
class Cycle {
public:
    Cycle();
    ~Cycle();
    void cycle(uint8_t c);
};
}