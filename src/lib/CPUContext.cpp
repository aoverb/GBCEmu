#include "CPUContext.hpp"
namespace GBCEmu {

// 根据寻址模式准备好数据
void CPUContext::fetchData()
{
    FUNC_TRACE("CPU::fetchData");
    uint16_t low, high;
    switch(curInst_.mode) {
        case AddrMode::IMP: return;
        case AddrMode::R:
            fetchedData_ = reg_.readReg(curInst_.reg1);
            return;
        case AddrMode::R_R:
            fetchedData_ = reg_.readReg(curInst_.reg2);
            return;
        case AddrMode::MR_R:
            fetchedData_ = reg_.readReg(curInst_.reg2);
            memoDest_ = bus_.read(reg_.readReg(curInst_.reg1));
            writeToMemo_ = true;
            return;
        case AddrMode::MR:
            memoDest_ = reg_.readReg(curInst_.reg1);
            writeToMemo_ = true;
            fetchedData_ = reg_.readReg(curInst_.reg1);
            return;
        case AddrMode::MR_D8:
            fetchedData_ = bus_.read(reg_.pc_);
            ++reg_.pc_;
            memoDest_ = reg_.readReg(curInst_.reg1);
            writeToMemo_ = true;
            return;
        case AddrMode::MC_R:
            fetchedData_ = reg_.readReg(curInst_.reg2);
            memoDest_ = reg_.readReg(RegType::C) | 0xFF00;
            writeToMemo_ = true;
            return;
        case AddrMode::R_MC:
            fetchedData_ = bus_.read(reg_.readReg(RegType::C) | 0xFF00);
            return;
        case AddrMode::R_HLI:
            fetchedData_ = bus_.read(reg_.readReg(RegType::HL));
            reg_.writeReg(RegType::HL, reg_.readReg(RegType::HL) + 1);
            return;
        case AddrMode::R_HLD:
            fetchedData_ = bus_.read(reg_.readReg(RegType::HL));
            reg_.writeReg(RegType::HL, reg_.readReg(RegType::HL) - 1);
            return;
        case AddrMode::HLI_R:
            fetchedData_ = reg_.readReg(curInst_.reg2);
            memoDest_ = reg_.readReg(RegType::HL);
            writeToMemo_ = true;
            reg_.writeReg(RegType::HL,reg_.readReg(RegType::HL) + 1);
            return;
        case AddrMode::HLD_R:
            fetchedData_ = reg_.readReg(curInst_.reg2);
            memoDest_ = reg_.readReg(RegType::HL);
            writeToMemo_ = true;
            reg_.writeReg(RegType::HL, reg_.readReg(RegType::HL) - 1);
            return;
        case AddrMode::R_D8:
            fetchedData_ = bus_.read(reg_.pc_);
            // emu_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::R_A8:
            fetchedData_ = bus_.read(bus_.read(reg_.pc_));
            // emu_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::A8_R:
            memoDest_ = bus_.read(reg_.pc_);
            fetchedData_ = reg_.readReg(curInst_.reg2);
            writeToMemo_ = true;
            // emu_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::R_HA8:
            fetchedData_ = 0xFF00 | bus_.read(bus_.read(reg_.pc_));
            // emu_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::HA8_R:
            memoDest_ = 0xFF00 | bus_.read(reg_.pc_);
            fetchedData_ = reg_.readReg(curInst_.reg2);
            writeToMemo_ = true;
            // emu_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::A16_R:
            low = bus_.read(reg_.pc_);
            // emu_.cycle(1);
            high = bus_.read(reg_.pc_ + 1);
            // emu_.cycle(1);
            memoDest_ = low | (high << 8);
            writeToMemo_ = true;
            fetchedData_ = reg_.readReg(curInst_.reg2);
            reg_.pc_ += 2;
            return;
        case AddrMode::R_A16:
            low = bus_.read(reg_.pc_);
            // emu_.cycle(1);
            high = bus_.read(reg_.pc_ + 1);
            // emu_.cycle(1);
            fetchedData_ = bus_.read(low | (high << 8));
            reg_.pc_ += 2;
            return;
        case AddrMode::R_D16:
        case AddrMode::D16:
            low = bus_.read(reg_.pc_);
            // emu_.cycle(1);
            high = bus_.read(reg_.pc_ + 1);
            // emu_.cycle(1);
            fetchedData_ = low | (high << 8);
            reg_.pc_ += 2;
            return;
        default:
            std::cout << "UNKNOWN ADDRESSING MODE \n";
            exit(-6);
    }
}
}