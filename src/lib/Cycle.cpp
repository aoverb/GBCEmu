#include "Cycle.hpp"

namespace GBCEmu {
Cycle::Cycle(EmuContext& emuContext, Timer& timer, DMA& dma, PPU& ppu) : emuContext_(emuContext), timer_(timer), dma_(dma), ppu_(ppu)
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
        }
        dma_.tick();
    }
}
}