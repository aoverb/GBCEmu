#pragma once
#include "LCDContext.hpp"
#include "PPU.hpp"
#include "DMA.hpp"

namespace GBCEmu {

class LCD {
public:
    LCD(DMA& dma);
    ~LCD();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    LCDContext& getContext();
protected:
    DMA& dma_;
    LCDContext lcdContext_;
};
}