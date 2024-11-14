#include "Bus.hpp"

// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers

namespace GBCEmu {
    Bus::Bus(Cartridge& cart) : cart_(cart)
    {
    }

    Bus::~Bus()
    {
    }
    uint8_t Bus::read(uint16_t addr)
    {
        TRACE("Bus::read, addr: " << std::hex << addr);
        if (addr < 0x8000) {
            return cart_.read(addr);
        }
        std::cerr << "Bus::read\n";
    }
    uint16_t Bus::read16(uint16_t addr)
    {
        TRACE("Bus::read, addr: " << std::hex << addr);
        if (addr < 0x8000) {
            return cart_.read(addr);
        }
        std::cerr << "Bus::read\n";
    }
    void Bus::write(uint16_t addr, uint8_t val)
    {
        if (addr < 0x8000) {
            cart_.write(addr, val);
            return;
        }
        std::cerr << "Bus::write\n";
    }
    void Bus::write16(uint16_t addr, uint16_t val)
    {
        write(addr, val & 0xFF);
        write(addr + 1, (val >> 8) & 0xFF);
    }
}