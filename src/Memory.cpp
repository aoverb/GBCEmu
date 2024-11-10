// src/Memory.cpp
#include "Memory.hpp"
#include <cstring>

Memory::Memory() {
    rom.resize(0x8000); // 初始化 ROM 大小（32KB）
    ram.resize(0x2000); // 初始化 RAM 大小（8KB）
}

Memory::~Memory() {}

// 读取一个字节
uint8_t Memory::readByte(uint16_t address) const {
    if (address < 0x8000) {
        // 读取 ROM
        if (address < rom.size()) {
            return rom[address];
        } else {
            return 0xFF;
        }
    } else if (address >= 0xC000 && address < 0xE000) {
        // 读取 RAM
        uint16_t ramAddress = address - 0xC000;
        if (ramAddress < ram.size()) {
            return ram[ramAddress];
        } else {
            return 0xFF;
        }
    }
    // 其他内存区域（VRAM, IO 等）可以扩展
    return 0xFF;
}

// 写入一个字节
void Memory::writeByte(uint16_t address, uint8_t value) {
    if (address >= 0xC000 && address < 0xE000) {
        // 写入 RAM
        uint16_t ramAddress = address - 0xC000;
        if (ramAddress < ram.size()) {
            ram[ramAddress] = value;
        }
    }
    // 其他内存区域（VRAM, IO 等）可以扩展
}

// 加载 ROM
void Memory::loadROM(const std::vector<char>& romData) {
    size_t size = romData.size();
    rom.resize(size);
    std::memcpy(rom.data(), romData.data(), size);
    // TODO: 处理 ROM 的内存银行控制器（MBC）
}