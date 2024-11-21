#pragma once
#include "Common.hpp"

class BusRWInterface {
public:
    virtual ~BusRWInterface() = default;

    // Read from the device
    virtual uint8_t busRead(uint16_t addr) = 0;

    // Write to the device
    virtual void busWrite(uint16_t addr, uint8_t value) = 0;
};
