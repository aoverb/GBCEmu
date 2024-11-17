#pragma once
#include <stdint.h>
#include <string>
#include <exception>
#include <vector>
#include <iostream>
#include <unordered_map>

#define NO_IMPL std::cout << "NOT IMPLEMENTED" << std::endl; exit(-5);

namespace GBCEmu {

class logHelper {
public:
    logHelper(std::string funcName) : funcName_(funcName){
        // std::cout << funcName_ << " Entered...\n";
    }
    ~logHelper(){
        // std::cout << funcName_ << " Exited...\n";
    }
private:
    std::string funcName_;
};

#define FUNC_TRACE(funcName) logHelper log(funcName);
#define TRACE(log) while(0);

inline uint16_t reverse(uint16_t n) {
    return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}

inline int getBit(uint64_t n, uint64_t digit)
{
    return n & (1 << digit) ? 1 : 0;
}

inline void setBit(uint16_t& n, uint64_t digit, bool val)
{
    if (val) {
        n |= (1 << digit);
    } else {
        n &= ~(1 << digit);
    }
}

inline void setBit(uint8_t& n, uint64_t digit, bool val)
{
    if (val) {
        n |= (1 << digit);
    } else {
        n &= ~(1 << digit);
    }
}

inline bool between(uint64_t a, uint64_t b, uint64_t c)
{
    return (a <= b) && (b <= c);
}

void delay(uint32_t ms);
}