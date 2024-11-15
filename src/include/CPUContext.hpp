#pragma once
#include "Common.hpp"
#include "Bus.hpp"
#include "Instruction.hpp"
#include "CPURegister.hpp"
#include <functional>

namespace GBCEmu {

class CPUContext {
public:
    CPUContext(Bus& bus, CPURegister& reg) : bus_(bus), reg_(reg) {};
    ~CPUContext() {};
    // 寄存器

    Bus& bus_;
    CPURegister& reg_;
    uint8_t curOpcode_;
    uint16_t fetchedData_;
    Instruction curInst_;
    bool halt_;
    bool interruptEnabled_;
    uint16_t memoDest_;
    bool writeToMemo_ = false;

    // 根据寻址模式准备好数据
    void fetchData();

};

using ProcFun = std::function<void(CPUContext&)>;

}