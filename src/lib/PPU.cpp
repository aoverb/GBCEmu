#include "PPU.hpp"

namespace GBCEmu {
    PPU::PPU()
    {
    }

    PPU::~PPU()
    {
    }

    void PPU::tick()
    {
    }

    uint8_t PPU::readOAM(uint16_t addr)
    {
        return vram_[addr - 0xFE00];
    }
    void PPU::writeOAM(uint16_t addr, uint8_t val)
    {
        if (addr >= 0xFE00) {
            addr -= 0xFE00;
        }

        uint8_t* ptr = reinterpret_cast<uint8_t *>(oam_);
        ptr[addr] = val;
    }
    uint8_t PPU::readVRAM(uint16_t addr)
    {
        if (addr >= 0x8000) {
            addr -= 0x8000;
        }

        uint8_t* ptr = reinterpret_cast<uint8_t *>(oam_);
        return ptr[addr];
    }
    void PPU::writeVRAM(uint16_t addr, uint8_t val)
    {
        vram_[addr - 0x8000] = val;
    }

    uint8_t PPU::busRead(uint16_t addr)
    {
        if (addr >= 0x8000 && addr < 0xA000) {
            return readVRAM(addr);
        } else if (addr >= 0xFEA0 && addr < 0xFF00) {
            return readOAM(addr);
        }
        throw std::out_of_range("PPU::busRead out of range!");
    }
    void PPU::busWrite(uint16_t addr, uint8_t value)
    {
        if (addr >= 0x8000 && addr < 0xA000) {
            writeVRAM(addr, value);
            return;
        } else if (addr >= 0xFF80 && addr < 0xFFFF) {
            writeOAM(addr, value);
            return;
        }
        throw std::out_of_range("PPU::busWrite out of range!");
    }
}