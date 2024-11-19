#pragma once
#include "Common.hpp"
#include "Timer.hpp"
namespace GBCEmu {
class IO {
public:
    IO(Timer timer);
    ~IO();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
protected:
    Timer& timer_;
    char serialData[2];
};
}