// src/Memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <cstdint>

class Memory {
public:
    Memory();
    ~Memory();

    uint8_t readByte(uint16_t address) const;
    void writeByte(uint16_t address, uint8_t value);

    void loadROM(const std::vector<char>& romData);

private:
    std::vector<uint8_t> rom;
    std::vector<uint8_t> ram;
    // 其他内存区域（VRAM, IO, OAM等）可以根据需要添加
};

#endif // MEMORY_H