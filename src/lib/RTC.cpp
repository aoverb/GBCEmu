#include "RTC.hpp"

namespace GBCEmu {
void RTC::init()
{
    
}

void RTC::update(uint32_t deltaTime)
{
    if (!halted()) {
        time += deltaTime;
        if (!latched) {
            updateTimeReg();
        }
    }
}

void RTC::updateTimeReg()
{
    h = (time / 3600) / 24;
    m = (time / 60) % 60;
    s = time % 60;

    uint16_t days = (uint16_t)(time / 86400);
    dl = (uint8_t)(days & 0xFF);

    setBit(dh, 0, (days & 0x100));
    setBit(dh, 7, (days >= 512));

}
void RTC::updateTimestamp()
{
    time = s + m * 60 + h * 3600 + days() * 86400;
}
void RTC::latch()
{
    latched = !latched;
    if (!latched) {
        updateTimeReg();
    }
}
}