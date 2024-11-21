#pragma once
#include "PPU.hpp"
#include "Bus.hpp"
#include "Common.hpp"

namespace GBCEmu {
class DMA {
public:
    DMA::DMA(PPU& ppu, Bus& bus);
    ~DMA();
    void tick();
    void start(uint8_t highAddr);
    bool transferring();
protected:
    bool active_;
    uint8_t delay_;
    uint8_t highAddr_;
    uint8_t relAddr_;

    PPU& ppu_;
    Bus& bus_;
};
}