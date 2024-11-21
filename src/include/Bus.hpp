#pragma once
#include "Common.hpp"
#include "BusRWInterface.hpp"
#ifndef BUS_HPP
#define BUS_HPP
namespace GBCEmu {

static constexpr size_t AddressSpace = 0x10000; // 64KB address space

class Bus {
public:
    Bus();
    ~Bus();
    
    uint8_t read(uint16_t addr);
    uint16_t read16(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    void write16(uint16_t addr, uint16_t val);
    void regDevice(uint16_t start, uint16_t end, BusRWInterface& device);
};
}

#endif // BUS_HPP