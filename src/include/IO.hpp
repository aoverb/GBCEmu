#pragma once
#include "Common.hpp"
#include "Timer.hpp"

namespace GBCEmu {
class IO {
public:
    IO(Timer& timer, Interrupt& interrupt);
    ~IO();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
protected:
    Timer& timer_;
    Interrupt& interrupt_;
    char serialData[2];
};
}