#pragma once
#include "Common.hpp"
namespace GBCEmu {
class Timer {
public:
    Timer();
    ~Timer();
    void tick();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
protected:
    uint16_t div_;
    uint8_t tima_;
    uint8_t tma_;
    uint8_t tac_;
};
}