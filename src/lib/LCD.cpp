#pragma once
#include "LCD.hpp"

namespace GBCEmu {
    LCD::LCD(DMA& dma) : dma_(dma)
    {
        lcdContext_.lcdc_ = 0x91;
        lcdContext_.scrX_ = 0;
        lcdContext_.scrY_ = 0;
        lcdContext_.ly_ = 00;
        lcdContext_.lyc_ = 0;
        lcdContext_.bgPalette_ = 0xFC;
        lcdContext_.objPalette_[0] = 0xFF;
        lcdContext_.objPalette_[1] = 0xFF;
        lcdContext_.winX = 0;
        lcdContext_.winY = 0;

        for (int i = 0; i < 4; i++) {
            lcdContext_.bgColor_[i] = defaultColor[i];
            lcdContext_.sp1Color_[i] = defaultColor[i];
            lcdContext_.sp2Color_[i] = defaultColor[i];
        }
    }

    LCD::~LCD()
    {
    }
    uint8_t LCD::read(uint16_t addr)
    {
        uint8_t offset = (addr - 0xFF40);
        uint8_t* p = reinterpret_cast<uint8_t*>(&lcdContext_);
        if (offset != 4) {
            // std::cout << "read" << (int)offset << ": " << std::hex << (int)p[offset] <<  std::endl;
        }
        
        return p[offset];
    }
    void LCD::write(uint16_t addr, uint8_t val)
    {
        uint8_t offset = (addr - 0xFF40);
        uint8_t* p = reinterpret_cast<uint8_t*>(&lcdContext_);
        p[offset] = val;
        // std::cout << "write" << (int)offset << ": " << (int)val << std::endl;
        if (offset == 6) {
            dma_.start(val);
        }

        if (offset == 7) {
            lcdContext_.updatePalette(0, val);
        } else if (offset == 8) {
            lcdContext_.updatePalette(1, val & 0b11111100);
        } else if (offset == 9) {
            lcdContext_.updatePalette(2, val & 0b11111100);
        } 
    }
    LCDContext &LCD::getContext()
    {
        return lcdContext_;
    }
}
