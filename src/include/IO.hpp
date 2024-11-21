#pragma once
#include "Common.hpp"
#include "Timer.hpp"
#include "BusRWInterface.hpp"

namespace GBCEmu {
class IO : public BusRWInterface {
public:
    IO(Timer& timer, Interrupt& interrupt);
    ~IO();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    virtual uint8_t busRead(uint16_t addr) final;
    virtual void busWrite(uint16_t addr, uint8_t value) final;
protected:
    Timer& timer_;
    Interrupt& interrupt_;
    char serialData[2];
};
}