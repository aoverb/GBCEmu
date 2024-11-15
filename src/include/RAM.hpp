#pragma once
#include "Common.hpp"

namespace GBCEmu {
class RAM {
public:
    RAM();
    ~RAM();
    uint8_t readWRAM(uint16_t addr);
    void writeWRAM(uint16_t addr, uint8_t val);
    uint8_t readHRAM(uint16_t addr);
    void writeHRAM(uint16_t addr, uint8_t val);
protected:
    uint8_t wram_[0x2000];
    uint8_t hram_[0x80];
};
}