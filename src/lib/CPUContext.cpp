#include "CPUContext.hpp"
namespace GBCEmu {

CPUContext::CPUContext(Bus& bus, CPURegister& reg, Cycle& cycle) : bus_(bus), reg_(reg), cycle_(cycle) {
    PROCESSOR_[InstType::NOP] = [this]() { this->nop(); };
    PROCESSOR_[InstType::LD] = [this]() { this->ld(); };
    PROCESSOR_[InstType::LDH] = [this]() { this->ldh(); };
    PROCESSOR_[InstType::ADD] = [this]() { this->add(); };
    PROCESSOR_[InstType::ADC] = [this]() { this->adc(); };
    PROCESSOR_[InstType::SUB] = [this]() { this->sub(); };
    PROCESSOR_[InstType::SBC] = [this]() { this->sbc(); };
    PROCESSOR_[InstType::AND] = [this]() { this->and(); };
    PROCESSOR_[InstType::XOR] = [this]() { this->xor(); };
    PROCESSOR_[InstType::OR] = [this]() { this->or(); };
    PROCESSOR_[InstType::CP] = [this]() { this->cp(); };
    PROCESSOR_[InstType::CB] = [this]() { this->cb(); };
    PROCESSOR_[InstType::INC] = [this]() { this->inc(); };
    PROCESSOR_[InstType::DEC] = [this]() { this->dec(); };
    PROCESSOR_[InstType::EI] = [this]() { this->ei(); };
    PROCESSOR_[InstType::DI] = [this]() { this->di(); };
    PROCESSOR_[InstType::JP] = [this]() { this->jp(); };
    PROCESSOR_[InstType::RET] = [this]() { this->ret(); };
    PROCESSOR_[InstType::RETI] = [this]() { this->reti(); };
    PROCESSOR_[InstType::CALL] = [this]() { this->call(); };
    PROCESSOR_[InstType::RST] = [this]() { this->rst(); };
    PROCESSOR_[InstType::JR] = [this]() { this->jr(); };
    PROCESSOR_[InstType::XOR] = [this]() { this->xor(); };
    PROCESSOR_[InstType::POP] = [this]() { this->pop(); };
    PROCESSOR_[InstType::PUSH] = [this]() { this->push(); };
    PROCESSOR_[InstType::RRCA] = [this]() { this->rrca(); };
    PROCESSOR_[InstType::RLCA] = [this]() { this->rlca(); };
    PROCESSOR_[InstType::RRA] = [this]() { this->rra(); };
    PROCESSOR_[InstType::RLA] = [this]() { this->rla(); };
    PROCESSOR_[InstType::STOP] = [this]() { this->stop(); };
    PROCESSOR_[InstType::DAA] = [this]() { this->daa(); };
    PROCESSOR_[InstType::CPL] = [this]() { this->cpl(); };
    PROCESSOR_[InstType::SCF] = [this]() { this->scf(); };
    PROCESSOR_[InstType::CCF] = [this]() { this->ccf(); };
    PROCESSOR_[InstType::HALT] = [this]() { this->halt(); };
};


// 根据寻址模式准备好数据
bool CPUContext::checkCond()
{
    bool z = reg_.getZFlag();
    bool c = reg_.getCFlag();
    // std::cout << "zflag!"  << z << std::endl;
    switch (curInst_.cond) {
        case CondType::NONE: return true;
        case CondType::C: return c;
        case CondType::NC: return !c;
        case CondType::Z: return z;
        case CondType::NZ: return !z;
    }

    return false;
}

void CPUContext::nop()
{
    return;
}

void CPUContext::ldh()
{
    if (curInst_.reg1 == RegType::A) {
        reg_.writeReg(curInst_.reg1, bus_.read(0xFF00 | fetchedData_));
    } else {
        bus_.write(0xFF00 | fetchedData_, reg_.a_);
    }

    cycle_.cycle(1);
}

void CPUContext::ld()
{
    if (writeToMemo_) {
        if (is16bitReg(curInst_.reg2)) {
            bus_.write16(memoDest_, fetchedData_);
        } else {
            bus_.write(memoDest_, fetchedData_);
        }
        writeToMemo_ = false;
        return;
    }

    if (curInst_.mode == AddrMode::HL_SPR) {
        uint8_t hFlag = (reg_.readReg(curInst_.reg1) & 0xF) +
            (reg_.readReg(curInst_.reg2) & 0xF) >= 0x10;
        uint8_t cFlag = (reg_.readReg(curInst_.reg1) & 0xFF) +
            (reg_.readReg(curInst_.reg2) & 0xFF) >= 0x100;
        reg_.setFlags(0, 0, hFlag, cFlag);
        reg_.writeReg(curInst_.reg1, reg_.readReg(curInst_.reg2) +
            static_cast<int8_t>(fetchedData_));
        return;
    }
    reg_.writeReg(curInst_.reg1, fetchedData_);
}

void CPUContext::inc()
{
    if (is16bitReg(curInst_.reg1)) {
        // ... emu
    }
    uint16_t val = reg_.readReg(curInst_.reg1) + 1;
    if (writeToMemo_) {
        val = (bus_.read(fetchedData_) & 0xFF) + 1;
        bus_.write(fetchedData_, val);
    } else {
        reg_.writeReg(curInst_.reg1, val);
    }

    if ((curOpcode_ & 0x03) == 0x03) {
        return;
    }

    reg_.setFlags(val == 0, 0, (val & 0x0F) == 0, -1);
}

void CPUContext::dec()
{
    if (is16bitReg(curInst_.reg1)) {
        // ... emu
    }
    uint16_t val = reg_.readReg(curInst_.reg1) - 1;
    if (writeToMemo_) {
        val = (bus_.read(fetchedData_) & 0xFF) - 1;
        bus_.write(fetchedData_, val);
    } else {
        reg_.writeReg(curInst_.reg1, val);
    }

    if ((curOpcode_ & 0x0B) == 0x0B) {
        return;
    }
    std::cout << "dec set flag! val=" << val << std::endl;
    reg_.setFlags(val == 0, 1, (val & 0x0F) == 0x0F, -1);
}

uint8_t CPUContext::stackPop()
{
    return bus_.read(reg_.sp_++);
}

uint16_t CPUContext::stackPop16()
{
    uint16_t lo;
    lo = stackPop();
    uint16_t hi;
    hi = stackPop();
    return (hi << 8) | lo;
}

void CPUContext::stackPush(uint8_t val)
{
    bus_.write(--reg_.sp_, val);
}

void CPUContext::stackPush16(uint16_t val)
{
    stackPush((val >> 8) & 0xFF);
    stackPush(val & 0xFF);
}

void CPUContext::ei()
{
    enablingIME_ = true;
}

void CPUContext::di()
{
    interruptEnabled_ = false;
}

void CPUContext::go2(uint16_t addr, bool pushPC)
{
    if (checkCond()) {
        if (pushPC) {
            stackPush16(reg_.pc_);
        }
        reg_.pc_ = addr;
        // emu_.cycle(1);
    }
}

void CPUContext::jp()
{
    go2(fetchedData_, false);
}

void CPUContext::ret()
{
    if (curInst_.cond != CondType::NONE) {
        // emu_cycle
    }

    if (checkCond()) {
        uint16_t lo;
        lo = stackPop();
        uint16_t hi;
        hi = stackPop();
        uint16_t res = (hi << 8) | lo;
        reg_.pc_ = res;
    }
}

void CPUContext::reti()
{
    interruptEnabled_ = true;
    ret();
}

void CPUContext::call()
{
    go2(fetchedData_, true);
}

void CPUContext::rst()
{
    go2(curInst_.param, true);
}

void CPUContext::jr()
{
    int8_t rel = static_cast<int8_t>(fetchedData_ & 0xFF);
    uint16_t addr = reg_.pc_ + rel;
    go2(addr, false);
}

void CPUContext::pop()
{
    uint16_t lo;
    lo = stackPop();
    uint16_t hi;
    hi = stackPop();
    uint16_t res = (hi << 8) | lo;
    reg_.writeReg(curInst_.reg1,
        res & (curInst_.reg1 == RegType::AF ? 0xFFF0 : 0xFFFF));
}

void CPUContext::push()
{
    uint16_t hi;
    hi = (reg_.readReg(curInst_.reg1) >> 8) & 0xFF;
    stackPush(hi);
    uint16_t lo;
    lo = reg_.readReg(curInst_.reg1) & 0xFF;
    stackPush(lo);
}

void CPUContext::add()
{
    uint16_t val = reg_.readReg(curInst_.reg1) + fetchedData_;
    bool is16bit = is16bitReg(curInst_.reg1);
    if (is16bit) {
        // emu...
    }

    if (curInst_.reg1 == RegType::SP) {
        val = reg_.readReg(curInst_.reg1) + static_cast<int8_t>(fetchedData_);
    }

    int z = (val & 0xFF) == 0;
    int h = ((reg_.readReg(curInst_.reg1) & 0xF) + (fetchedData_ & 0xF)) >= 0x10;
    int c = (static_cast<int>(reg_.readReg(curInst_.reg1) & 0xFF) + static_cast<int>(fetchedData_ & 0xFF)) >= 0x100;
    if (is16bit) {
        z = -1;
        h = ((reg_.readReg(curInst_.reg1) & 0xFFF) + (static_cast<int8_t>(fetchedData_) & 0xFFF)) >= 0x1000;
        c = (static_cast<uint32_t>(reg_.readReg(curInst_.reg1) & 0xFFFF) + static_cast<uint32_t>(fetchedData_ & 0xFFFF)) >= 0x10000;
    }

    if (curInst_.reg1 == RegType::SP) {
        z = 0;
        h = ((reg_.readReg(curInst_.reg1) & 0xF) + (static_cast<int8_t>(fetchedData_) & 0xF)) >= 0x10;
        c = (static_cast<uint16_t>(reg_.readReg(curInst_.reg1) & 0xFF) + static_cast<uint16_t>(fetchedData_ & 0xFF)) >= 0x100;
    }

    reg_.writeReg(curInst_.reg1, val & 0xFFFF);
    reg_.setFlags(z, 0, h, c);
}

void GBCEmu::CPUContext::sub()
{
    uint16_t val = reg_.readReg(curInst_.reg1) - fetchedData_;

    int z = val == 0;
    int h = ((static_cast<int>(reg_.readReg(curInst_.reg1)) & 0xF) - (static_cast<int>(fetchedData_) & 0xF)) < 0;
    int c = (static_cast<int>(reg_.readReg(curInst_.reg1)) - static_cast<int>(fetchedData_)) < 0;

    reg_.writeReg(curInst_.reg1, val);
    reg_.setFlags(z, 1, h, c);
}

void GBCEmu::CPUContext::sbc()
{
    uint8_t val = fetchedData_ + reg_.getCFlag();

    int z = (reg_.readReg(curInst_.reg1) - val) == 0;
    int h = ((static_cast<int>(reg_.readReg(curInst_.reg1)) & 0xF) - (static_cast<int>(fetchedData_) & 0xF) - (static_cast<int>(reg_.getCFlag()))) < 0;
    int c = (static_cast<int>(reg_.readReg(curInst_.reg1)) - static_cast<int>(fetchedData_)) - reg_.getCFlag() < 0;

    reg_.writeReg(curInst_.reg1, (reg_.readReg(curInst_.reg1) - val));
    reg_.setFlags(z, 1, h, c);
}

void GBCEmu::CPUContext::adc()
{
    uint16_t u = fetchedData_;
    uint16_t a = reg_.a_;
    uint16_t c = reg_.getCFlag();

    reg_.a_ = (a + u + c) & 0xFF;
    reg_.setFlags(a == 0, 0, (a & 0xF) + (u & 0xF) + c > 0xF, a + u + c > 0xFF);
}

void CPUContext::and()
{
    reg_.a_ &= fetchedData_;
    reg_.setFlags(reg_.a_ == 0, 0, 1, 0);
}

void CPUContext::xor()
{
    reg_.a_ ^= fetchedData_ & 0xFF;
    reg_.setFlags(reg_.a_ == 0, 0, 0, 0);
}

void CPUContext::or()
{
    reg_.a_ |= fetchedData_ & 0xFF;
    reg_.setFlags(reg_.a_ == 0, 0, 0, 0);
}

void CPUContext::cp()
{
    int n = static_cast<int>(reg_.a_) - static_cast<int>(fetchedData_);
    // std::cout << "n: " << n << "\n"; 
    reg_.setFlags(n == 0, 1, (static_cast<int>(reg_.a_) & 0x0F) - (static_cast<int>(fetchedData_) & 0x0F), n < 0);
}

const RegType regTable[8] = {
    RegType::B,
    RegType::C,
    RegType::D,
    RegType::E,
    RegType::H,
    RegType::L,
    RegType::HL,
    RegType::A,
};

RegType CPUContext::decodeReg(uint8_t reg)
{
    return regTable[reg];
}

void CPUContext::cb()
{
    uint8_t opcode = fetchedData_;
    RegType reg = decodeReg(opcode & 0b111);
    uint8_t bit = (opcode >> 3) & 0b111;
    uint8_t bitOp = (opcode >> 6) & 0b11;
    uint8_t regVal = reg_.readReg(reg);

    cycle_.cycle(1);

    if (reg == RegType::HL) {
        cycle_.cycle(2);
    }

    switch (bitOp) {
        case 1: // BIT
            reg_.setFlags(getBit(regVal, bit) == 0, 0, 1, -1);
            return;
        case 2: // RST
            reg_.writeReg(reg, regVal & ~(1 << bit));
            return;
        case 3: // SET
            reg_.writeReg(reg, regVal | (1 << bit));
            return;
    }

    bool flagC = reg_.getCFlag();
    switch (bitOp) {
        case 0: {// RLC
            uint8_t carry = getBit(regVal, 7);
            uint8_t res = (regVal << 1) | carry & 0xFF;
            reg_.writeReg(reg, res);
            reg_.setFlags(res == 0, 1, -1, carry);
            return;
            }
        case 1: {// RRC
            uint8_t carry = getBit(regVal, 0);
            uint8_t res = (regVal >> 1) | (carry << 7  & 0xFF);
            reg_.writeReg(reg, res);
            reg_.setFlags(res == 0, 0, 0, carry);
            return;
            }
        case 2: {// RL
            uint8_t res = (regVal << 1) | flagC;
            reg_.writeReg(reg, res);
            reg_.setFlags(res == 0, 0, 0, !!(regVal & 0x80));
            return;
            }
        case 3: {// RR
            uint8_t res = (regVal >> 1) | (flagC << 7);
            reg_.writeReg(reg, res);
            reg_.setFlags(res == 0, 0, 0, regVal & 0x1);
            return;
            }
        case 4: {// SLA
            uint8_t res = (regVal << 1);
            reg_.writeReg(reg, res);
            reg_.setFlags(res == 0, 0, 0, !!(regVal & 0x80));
            return;
            }
        case 5: {// SRA
            uint8_t res = (static_cast<int>(regVal) >> 1);
            reg_.writeReg(reg, res);
            reg_.setFlags(res == 0, 0, 0, regVal & 0x1);
            return;}
        case 6: {// SWAP
            uint8_t res = (regVal >> 4) | ((regVal & 0xF) << 4);
            reg_.writeReg(reg, res);
            reg_.setFlags(res == 0, 0, 0, 0);
            return;
            }
        case 7: {// SRL
            uint8_t res = (regVal >> 1);
            reg_.writeReg(reg, res);
            reg_.setFlags(res == 0, 0, 0, regVal & 0x1);
            return;
            }
    }

    std::cerr << "INVALID CB\n";
    NO_IMPL
}

void CPUContext::rlca()
{
    uint8_t res = reg_.a_;
    bool cFlag = (res >> 7) & 1;
    res = (res << 1) | cFlag;
    reg_.a_ = res;
    reg_.setFlags(0, 0, 0, cFlag);
}

void CPUContext::rrca()
{
    uint8_t res = reg_.a_;
    uint8_t lsb = res & 1;
    res = (res >> 1) | (lsb << 7);
    reg_.a_ = res;
    reg_.setFlags(0, 0, 0, lsb);
}

void CPUContext::rla()
{
    uint8_t res = reg_.a_;
    bool cFlag = (res >> 7) & 1;
    res = (res << 1) | reg_.getCFlag();
    reg_.a_ = res;
    reg_.setFlags(0, 0, 0, cFlag);
}

void CPUContext::rra()
{
    uint8_t res = reg_.a_;
    bool cFlag = res & 1;
    res = (res >> 1) | (reg_.getCFlag() << 7);
    reg_.a_ = res;
    reg_.setFlags(0, 0, 0, cFlag);
}

void CPUContext::stop()
{
    std::cerr << "STOP INSTRUCTION!\n";
    exit(0);
}

void CPUContext::daa()
{
    uint8_t res = 0;
    int cFlag = 0;

    if (reg_.getHFlag() || (!reg_.getNFlag() && (reg_.a_ & 0xF) > 9)) {
        res = 6;
    }
    if (reg_.getCFlag() || (!reg_.getNFlag() && reg_.a_ > 0x99)) {
        res |= 0x60;
        cFlag = 1;
    }

    reg_.a_ += reg_.getNFlag() ? -res : res;
    reg_.setFlags(reg_.a_ == 0, -1, 0, cFlag);
}

void CPUContext::cpl()
{
    reg_.a_ = ~reg_.a_;
    reg_.setFlags(-1, 1, 1, -1);
}

void CPUContext::scf()
{
    reg_.setFlags(-1, 0, 0, -1);
}

void CPUContext::ccf()
{
    reg_.setFlags(-1, 0, 0, ~reg_.getCFlag());
}

void CPUContext::halt()
{
    halt_ = true;
}

void CPUContext::process()
{
    auto it = PROCESSOR_.find(curInst_.type);
    if(it != PROCESSOR_.end()) {
        (it->second)();
        return;
    }
    throw std::runtime_error("failed to find processor of instType:" + static_cast<int>(curInst_.type));
}

void CPUContext::requestInterrupt(InterruptType interrupt)
{
    
}

void CPUContext::handleInterrupt()
{
    if ((intFlag_ & static_cast<uint8_t>(InterruptType::VBLANK)) && (reg_.ie_ & static_cast<uint8_t>(InterruptType::VBLANK))) {
        handleByAddress(0x40);
        intFlag_ &= ~static_cast<uint8_t>(InterruptType::VBLANK);
        halt_ = false;
        interruptEnabled_ = false;
    } else if ((intFlag_ & static_cast<uint8_t>(InterruptType::LCD_STAT)) && (reg_.ie_ & static_cast<uint8_t>(InterruptType::LCD_STAT))) {
        handleByAddress(0x48);
        intFlag_ &= ~static_cast<uint8_t>(InterruptType::LCD_STAT);
        halt_ = false;
        interruptEnabled_ = false;
    } else if ((intFlag_ & static_cast<uint8_t>(InterruptType::TIMER)) && (reg_.ie_ & static_cast<uint8_t>(InterruptType::TIMER))) {
        handleByAddress(0x50);
        intFlag_ &= ~static_cast<uint8_t>(InterruptType::TIMER);
        halt_ = false;
        interruptEnabled_ = false;
    } else if ((intFlag_ & static_cast<uint8_t>(InterruptType::SERIAL)) && (reg_.ie_ & static_cast<uint8_t>(InterruptType::SERIAL))) {
        handleByAddress(0x58);
        intFlag_ &= ~static_cast<uint8_t>(InterruptType::SERIAL);
        halt_ = false;
        interruptEnabled_ = false;
    } else if ((intFlag_ & static_cast<uint8_t>(InterruptType::JOYPAD)) && (reg_.ie_ & static_cast<uint8_t>(InterruptType::JOYPAD))) {
        handleByAddress(0x60);
        intFlag_ &= ~static_cast<uint8_t>(InterruptType::JOYPAD);
        halt_ = false;
        interruptEnabled_ = false;
    } 
}

void CPUContext::handleByAddress(uint16_t addr)
{
    stackPush16(reg_.pc_);
    reg_.pc_ = addr;
}

void CPUContext::fetchData()
{
    memoDest_ = 0;
    writeToMemo_ = false;
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
        case AddrMode::D8:
            fetchedData_ = bus_.read(reg_.pc_);
            ++reg_.pc_;
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
            cycle_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::R_A8:
            fetchedData_ = bus_.read(bus_.read(reg_.pc_));
            cycle_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::A8_R:
            memoDest_ = bus_.read(reg_.pc_) | 0xFF00;
            writeToMemo_ = true;
            cycle_.cycle(1);
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
            cycle_.cycle(1);
            high = bus_.read(reg_.pc_ + 1);
            cycle_.cycle(1);
            fetchedData_ = low | (high << 8);
            reg_.pc_ += 2;
            return;
        default:
            std::cout << "UNKNOWN ADDRESSING MODE \n";
            exit(-6);
    }
}
}