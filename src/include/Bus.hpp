#pragma once
#include "common.hpp"
namespace GBCEmu {
class Bus {
public:
    Bus();
    ~Bus();

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
};
}