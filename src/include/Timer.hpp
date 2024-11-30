#pragma once
#include "Common.hpp"
#include "Interrupt.hpp"

namespace GBCEmu {
class Timer {
public:
    Timer(Interrupt& interrupt);
    ~Timer();
    void tick();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    uint8_t getDiv();
protected:
    uint16_t div_ = 0;
    uint8_t tima_;
    uint8_t tma_;
    uint8_t tac_;
    Interrupt& interrupt_;
};
}