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
}