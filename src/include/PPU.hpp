#pragma once
#include "Common.hpp"
namespace GBCEmu {

typedef struct OAM {
    uint8_t x;
    uint8_t y;
    uint8_t tile;

    unsigned cgbPaletteNum : 3;
    unsigned cgbVRAMBank : 1;
    unsigned paletteNum : 1;
    unsigned xFlip : 1;
    unsigned yFlip : 1;
    unsigned prior : 1;
};

class PPU {
public:
    PPU();
    ~PPU();
    void tick();
    uint8_t readOAM(uint16_t addr);
    void writeOAM(uint16_t addr, uint8_t val);
    uint8_t readVRAM(uint16_t addr);
    void writeVRAM(uint16_t addr, uint8_t val);
protected:
    OAM oam_[40];
    uint8_t vram_[0x2000];

};
}