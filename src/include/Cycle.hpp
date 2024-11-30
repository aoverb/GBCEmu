#pragma once
#include "Common.hpp"
#include "Timer.hpp"
#include "EmuContext.hpp"
#include "DMA.hpp"
#include "PPU.hpp"
#include "APU.hpp"

namespace GBCEmu {
class Cycle {
public:
    Cycle(EmuContext& emuContext, Timer& timer, DMA& dma, PPU& ppu, APU& apu);
    ~Cycle();
    void cycle(uint8_t c);
protected:
    EmuContext& emuContext_;
    Timer& timer_;
    DMA& dma_;
    PPU& ppu_;
    APU& apu_;
};
}