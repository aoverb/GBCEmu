#pragma once
#include "Common.hpp"
#include "BusRWInterface.hpp"

namespace GBCEmu {
class RAM : public BusRWInterface{
public:
    RAM();
    ~RAM();
    uint8_t readWRAM(uint16_t addr);
    void writeWRAM(uint16_t addr, uint8_t val);
    uint8_t readHRAM(uint16_t addr);
    void writeHRAM(uint16_t addr, uint8_t val);
    virtual uint8_t busRead(uint16_t addr) final;
    virtual void busWrite(uint16_t addr, uint8_t value) final;
protected:
    uint8_t wram_[0x2000];
    uint8_t hram_[0x80];
};
}