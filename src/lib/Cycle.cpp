#include "Cycle.hpp"
#include <chrono>

namespace GBCEmu {
Cycle::Cycle(EmuContext& emuContext, Timer& timer, DMA& dma, PPU& ppu, APU& apu) :
    emuContext_(emuContext), timer_(timer), dma_(dma), ppu_(ppu), apu_(apu)
{
}

Cycle::~Cycle()
{
}

void Cycle::cycle(uint8_t c)
{
    for (int i = 0; i < c; i++) {
        int n = 4;
        while (n--) {
            emuContext_.ticks++;
            timer_.tick();
            ppu_.tick();
            apu_.tick(timer_);
        }
        dma_.tick();
    }
    if (emuContext_.ticks % 1048576 == 0) {
        emuContext_.fps = ppu_.getFPS();
    }
}
}