#pragma once
#include "Common.hpp"

namespace GBCEmu {
#pragma pack(push, 1)
struct RTC {
    uint8_t s = 0;
    uint8_t m = 0;
    uint8_t h = 0;
    uint8_t dl = 0;
    uint8_t dh = 0;

    uint32_t time = 0;
    bool latched = false;
    bool latching = false;

    void init();
    void update(uint32_t deltaTime);
    void updateTimeReg();
    void updateTimestamp();
    void latch();
    uint16_t days() const
    {
        return static_cast<uint16_t>(dl) + (static_cast<uint16_t>(dh & 0x01)) << 8;
    }
    bool halted() const
    {
        return getBit(dh, 6);
    }
    bool dayOverFlow() const
    {
        return getBit(dh, 7);
    }
};
#pragma pack(pop)
}