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
    Bus::Bus(Cartridge& cart, RAM& ram, CPURegister& reg) : cart_(cart), ram_(ram), reg_(reg)
    {
    }

    Bus::~Bus()
    {
    }
    uint8_t Bus::read(uint16_t addr)
    {
        TRACE("Bus::read, addr: " << std::hex << addr);
        if (addr < 0x8000) {
            return cart_.read(addr);
        } else if (addr < 0xA000) {
            std::cerr << "Bus::read unspported..." << std::hex << addr << "\n";
        } else if (addr < 0xC000) {
            return cart_.read(addr);
        } else if (addr < 0xE000) {
            return ram_.readWRAM(addr);
        } else if (addr < 0xFE00) {
            std::cerr << "Bus::read unspported..." << std::hex << addr << "\n";
        } else if (addr < 0xFEA0) {
            std::cerr << "Bus::read unspported..." << std::hex << addr << "\n";
            return 0;
        } else if (addr < 0xFF00) {
            std::cerr << "Bus::read unspported..." << std::hex << addr << "\n";

        } else if (addr < 0xFF80) {
            std::cerr << "Bus::read unspported..." << std::hex << addr << "\n";
        } else if (addr == 0xFFFF) {
            return reg_.ie_;
        } else {
            ram_.readHRAM(addr);
        }
        return 0;
    }
    uint16_t Bus::read16(uint16_t addr)
    {
        TRACE("Bus::read16, addr: " << std::hex << addr);
        if (addr < 0x8000) {
            return static_cast<uint16_t>(cart_.read(addr)) | (static_cast<uint16_t>(cart_.read(addr + 1)) << 8);
        }
        std::cerr << "Bus::read16 unsupported address: " << std::hex << addr << "\n";
        throw std::out_of_range("Bus::read16 unsupported address");
    }
    void Bus::write(uint16_t addr, uint8_t val)
    {
        // std::cout << "Bus::write addr: " << std::hex << addr << "\n";
        if (addr < 0x8000) {
            cart_.write(addr, val);
            return;
        } else if (addr < 0xA000) {
            std::cerr << "Bus::write unspported..." << std::hex << addr << "\n";
        } else if (addr < 0xC000) {
            cart_.write(addr, val);
        } else if (addr < 0xE000) {
            ram_.writeWRAM(addr, val);
            return;
        } else if (addr < 0xFE00) {
            std::cerr << "Bus::write unspported..." << std::hex << addr << "\n";
        } else if (addr < 0xFEA0) {
            std::cerr << "Bus::write unspported..." << std::hex << addr << "\n";
        } else if (addr < 0xFF00) {
            std::cerr << "Bus::write unspported..." << std::hex << addr << "\n";
        } else if (addr < 0xFF80) {
            std::cerr << "Bus::write unspported..." << std::hex << addr << "\n";
        } else if (addr == 0xFFFF) {
            reg_.ie_ = val; 
        } else  {
            ram_.writeHRAM(addr, val);
        }
    }
    void Bus::write16(uint16_t addr, uint16_t val)
    {
        write(addr, val & 0xFF);
        write(addr + 1, (val >> 8) & 0xFF);
    }
}