// src/CPU.cpp
#include "CPU.hpp"
#include <iostream>
#include <utility>

namespace GBCEmu {
// 构造函数

// 析构函数
CPU::~CPU() {}

// 重置 CPU
void CPU::reset()
{
    a_ = f_ = 0;
    b_ = c_ = d_ = e_ = h_ = l_ = 0;
    sp_ = 0xFFFE; // 初始化堆栈指针
    pc_ = 0x0100; // 游戏开始执行地址
    halt_ = false;
}

void CPU::fetchInst()
{
    try {
        curOpcode_ = bus_.read(pc_++);
        curInst_ = ::std::move(getInstructionByOpCode(curOpcode_));
    } catch (std::exception ex) {
        std::cerr << "CPU::fetchInst catches exception: " << ex.what() << "\n";
    } catch (...) {
        std::cerr << "CPU::fetchInst catches exception!\n";
    }
}

void CPU::fetchData()
{
    uint16_t low, high;
    switch(curInst_.mode) {
        case AddrMode::IMP: return;
        case AddrMode::R:
            fetchedData_ = readReg(curInst_.reg1);
            return;
        case AddrMode::R_D8:
            fetchedData_ = bus_.read(pc_);
            // emu_.cycle(1);
            ++pc_;
            return;
        case AddrMode::D16:
            low = bus_.read(pc_);
            // emu_.cycle(1);
            high = bus_.read(pc_ + 1);
            // emu_.cycle(1);
            fetchedData_ = low | (high << 8);
            pc_ += 2;
            return;
        case AddrMode::R_R:
            
        default:
            std::cout << "UNKNOWN ADDRESSING MODE \n";
            exit(-6);
    }
}

uint16_t CPU::readReg(RegType reg)
{
    switch(reg) {
        case RegType::A: return a_;
        case RegType::B: return b_;
        case RegType::C: return c_;
        case RegType::D: return d_;
        case RegType::E: return e_;
        case RegType::F: return f_;
        case RegType::H: return h_;
        case RegType::L: return l_;
    }    
}

void CPU::execute()
{
    std::cout << "opcode: " << std::hex << (int)curOpcode_ << "\t" << "pc:" << pc_ << "\n";
    std::cout << "no execution...\n";
}

// 执行下一条指令
bool CPU::step()
{
    if (!halt_) {
        fetchInst();
        fetchData();
        execute();
    }
    return true;
}
}