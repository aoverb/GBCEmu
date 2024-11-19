#pragma once
#include "Common.hpp"
#include "Timer.hpp"
#include "EmuContext.hpp"

namespace GBCEmu {
class Cycle {
public:
    Cycle(EmuContext& emuContext, Timer& timer);
    ~Cycle();
    void cycle(uint8_t c);
protected:
    EmuContext& emuContext_;
    Timer& timer_;
};
}