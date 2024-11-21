#pragma once
#include "Common.hpp"
#include "Cartridge.hpp"
#include "CPURegister.hpp"
#include "RAM.hpp"
#include "PPU.hpp"
#include "IO.hpp"
#include "Interrupt.hpp"

namespace GBCEmu {
class Bus {
public:
    Bus(Cartridge& cart, RAM& ram, CPURegister& reg, Interrupt& interrupt, IO& io, PPU& ppu);
    ~Bus();
    
    uint8_t read(uint16_t addr);
    uint16_t read16(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    void write16(uint16_t addr, uint16_t val);
protected:
    Cartridge& cart_;
    RAM& ram_;
    IO& io_;
    PPU& ppu_;
    CPURegister& reg_;
    Interrupt& interrupt_;
    
};
}