#include "Bus.hpp"

// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers

namespace GBCEmu {
    static BusRWInterface* deviceArray_[65536] = {nullptr};
    Bus::Bus()
    {

    }

    Bus::~Bus()
    {
    }

    void Bus::regDevice(uint16_t start, uint16_t end, BusRWInterface& device) {
        if (start > end) {
            throw std::invalid_argument("Invalid address range or null device");
        }

        for (uint32_t addr = start; addr <= end; ++addr) {
            deviceArray_[addr] = &device;
        }
    }

    uint8_t Bus::read(uint16_t addr)
    {
        // std::cout << "Bus::read, addr: " << addr << std::endl;
        auto device = deviceArray_[addr];
        if (device) {
            return device->busRead(addr);
        } else {
            return 0;
        }
        throw std::out_of_range("Address out of range or unregistered");
    }

    uint16_t Bus::read16(uint16_t addr)
    {
        TRACE("Bus::read16, addr: " << std::hex << addr);
        if (addr < 0x8000) {
            return static_cast<uint16_t>(read(addr)) | (static_cast<uint16_t>(read(addr + 1)) << 8);
        }
        std::cerr << "Bus::read16 unsupported address: " << std::hex << addr << "\n";
        throw std::out_of_range("Bus::read16 unsupported address");
    }

    void Bus::write(uint16_t addr, uint8_t val)
    {
        auto device = deviceArray_[addr];
        if (device) {
            device->busWrite(addr, val);
            return;
        } else {
            return;
        }
        throw std::out_of_range("Address out of range or unregistered");
    }

    void Bus::write16(uint16_t addr, uint16_t val)
    {
        write(addr, val & 0xFF);
        write(addr + 1, (val >> 8) & 0xFF);
    }
}