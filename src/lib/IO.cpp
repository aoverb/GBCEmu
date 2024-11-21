#include "IO.hpp"

namespace GBCEmu {
    IO::IO(Timer& timer, Interrupt& interrupt) : timer_(timer), interrupt_(interrupt)
    {
    }

    IO::~IO()
    {
    }

    uint8_t IO::read(uint16_t addr)
    {
        static uint8_t ranno = 0x95;
        if (addr == 0xFF01) {
            return serialData[0];
        }
        if (addr == 0xFF02) {
            return serialData[1];
        }
        if (between(addr, 0xFF04, 0xFF07)) {
            return timer_.read(addr);
        }

        if (addr == 0xFF0F) {
            return interrupt_.getIntFlag();
        }
        // std::cerr << "CPURegister::ioRead UNSUPPORTED: " << std::hex << addr << "\n";
        return ranno++;
    }

    void IO::write(uint16_t addr, uint8_t val)
    {
        if (addr == 0xFF01) {
            serialData[0] = val;
            return;
        }
        if (addr == 0xFF02) {
            serialData[1] = val;
            return;
        }

        if (between(addr, 0xFF04, 0xFF07)) {
            timer_.write(addr, val);
            return;
        } 
        if (addr == 0xFF0F) {
            interrupt_.setIntFlag(val);
            return;
        }

        if (addr == 0xFF46) {
            // dma_.start(val);
            // std::cerr << "DMA Start!" << std::endl;
        }
        // std::cerr << "CPURegister::ioWrite UNSUPPORTED: " << std::hex << (int)addr << std::endl;
    }
    uint8_t IO::busRead(uint16_t addr)
    {
        return read(addr);
    }
    void IO::busWrite(uint16_t addr, uint8_t value)
    {
        write(addr, value);
    }
}