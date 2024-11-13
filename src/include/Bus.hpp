#pragma once
#include "common.hpp"
#include "Cartridge.hpp"

namespace GBCEmu {
class Bus {
public:
    Bus(Cartridge& cart);
    ~Bus();

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    void write16(uint16_t addr, uint16_t val);
protected:
    Cartridge& cart_;
};
}