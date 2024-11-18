#include "RAM.hpp"

namespace GBCEmu {
GBCEmu::RAM::RAM()
{
}

GBCEmu::RAM::~RAM()
{
}

uint8_t RAM::readWRAM(uint16_t addr)
{
    if (addr < 0xC000 || addr >= 0xE000) {
        throw std::out_of_range("RAM::readWRAM out of range!");
    }
    return wram_[addr - 0xC000];
}
void RAM::writeWRAM(uint16_t addr, uint8_t val)
{
    if (addr < 0xC000 || addr >= 0xE000) {
        throw std::out_of_range("RAM::writeWRAM out of range!");
    }
    wram_[addr - 0xC000] = val;
    // std::cout << "stack push " << std::hex << (int)val << "to addr " << (int)addr << std::endl;
}
uint8_t RAM::readHRAM(uint16_t addr)
{
    if (addr < 0xFF80 || addr >= 0xFFFF) {
        throw std::out_of_range("RAM::readHRAM out of range!");
    }
    return hram_[addr - 0xFF80];
}
void RAM::writeHRAM(uint16_t addr, uint8_t val)
{
    if (addr < 0xFF80 || addr >= 0xFFFF) {
        throw std::out_of_range("RAM::writeHRAM out of range!");
    }
     
    hram_[addr - 0xFF80] = val;
}
}