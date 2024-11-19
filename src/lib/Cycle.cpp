#include "Cycle.hpp"

namespace GBCEmu {
Cycle::Cycle(EmuContext& emuContext, Timer& timer) : emuContext_(emuContext), timer_(timer)
{
}

Cycle::~Cycle()
{
}

void Cycle::cycle(uint8_t c)
{
    int n = c * 4;

    for (int i = 0; i < n; ++i) {
        ++emuContext_.ticks;
        timer_.tick();
    }
}
}