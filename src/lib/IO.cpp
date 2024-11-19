#include "IO.hpp"

namespace GBCEmu {
    IO::IO(Timer timer) : timer_(timer)
    {
    }

    IO::~IO()
    {
    }

    uint8_t IO::read(uint16_t addr)
    {
        if (addr == 0xFF01) {
            return serialData[0];
        }
        if (addr == 0xFF02) {
            return serialData[1];
        }
        // std::cerr << "CPURegister::ioRead UNSUPPORTED\n";
        return 0;
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
        // std::cerr << "CPURegister::ioWrite UNSUPPORTED: " << std::hex << (int)addr << std::endl;
    }
}