#pragma once
#include "Common.hpp"

namespace GBCEmu {
typedef struct {
    bool paused;
    bool running;
    bool die = false;
    uint64_t ticks;
} EmuContext;
}