#pragma once
#include "Common.hpp"

namespace GBCEmu {
typedef struct {
    bool paused;
    bool running;
    bool die = false;
    int fps = 0;
    uint64_t ticks = 0;
} EmuContext;
}