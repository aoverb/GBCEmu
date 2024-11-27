#include "DMA.hpp"

namespace GBCEmu {
DMA::DMA(PPU& ppu, Bus& bus) : ppu_(ppu), bus_(bus)
{}

DMA::~DMA()
{
}

void DMA::tick()
{
    if (!active_) {
        return;
    }

    if (delay_) {
        --delay_;
        return;
    }

    ppu_.writeOAM(relAddr_, bus_.read(highAddr_* 0x100 + (relAddr_)));

    relAddr_++;

    active_ = relAddr_ < 0xA0;
}

void DMA::start(uint8_t highAddr)
{
    highAddr = highAddr_;
    active_ = true;
    relAddr_ = 0;
    delay_ = 2;
    // std::cout << "DMA Start!" << std::endl;
}
bool DMA::transferring()
{
    return active_;
}
}