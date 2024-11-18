// src/CPU.cpp
#include "CPU.hpp"
#include <iostream>
#include <utility>
#include "dbg.hpp"

namespace GBCEmu {


namespace Processor {

}

// 析构函数
CPU::~CPU() {}

// 重置 CPU
void CPU::reset()
{
    context_.reg_.a_ = 0x01;
    context_.reg_.b_ = 0x00;
    context_.reg_.c_ = 0x13;
    context_.reg_.d_ = 0x00;
    context_.reg_.e_ = 0xD8;
    context_.reg_.h_ = 0x01;
    context_.reg_.f_ = 0xB0;
    context_.reg_.l_ = 0x4D;
    context_.reg_.b_ = 0;
    context_.reg_.sp_ = 0xFFFE; // 初始化堆栈指针
    context_.reg_.pc_ = 0x0100; // 游戏开始执行地址
    context_.halt_ = false;
    context_.interruptEnabled_ = true;
}


char *inst_lookup[] = {
    "<NONE>",
    "NOP",
    "LD",
    "INC",
    "DEC",
    "RLCA",
    "ADD",
    "RRCA",
    "STOP",
    "RLA",
    "JR",
    "RRA",
    "DAA",
    "CPL",
    "SCF",
    "CCF",
    "HALT",
    "ADC",
    "SUB",
    "SBC",
    "AND",
    "XOR",
    "OR",
    "CP",
    "POP",
    "JP",
    "PUSH",
    "RET",
    "CB",
    "CALL",
    "RETI",
    "LDH",
    "JPHL",
    "DI",
    "EI",
    "RST",
    "IN_ERR",
    "IN_RLC", 
    "IN_RRC",
    "IN_RL", 
    "IN_RR",
    "IN_SLA", 
    "IN_SRA",
    "IN_SWAP", 
    "IN_SRL",
    "IN_BIT", 
    "IN_RES", 
    "IN_SET"
};

char *inst_name(InstType t) {
    return inst_lookup[(int)t];
}

static char *rt_lookup[] = {
    "<NONE>",
    "A",
    "F",
    "B",
    "C",
    "D",
    "E",
    "H",
    "L",
    "AF",
    "BC",
    "DE",
    "HL",
    "SP",
    "PC"
};

void CPU::inst_to_str(char *str) {
    sprintf(str, "%s ", inst_name(context_.curInst_.type));

    switch(context_.curInst_.mode) {
        case AddrMode::IMP:
            return;

        case AddrMode::R_D16:
        case AddrMode::R_A16:
            sprintf(str, "%s %s,$%04X", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)], context_.fetchedData_);
            return;

        case AddrMode::R:
            sprintf(str, "%s %s", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)]);
            return;

        case AddrMode::R_R: 
            sprintf(str, "%s %s,%s", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)], rt_lookup[static_cast<int>(context_.curInst_.reg2)]);
            return;

        case AddrMode::MR_R:
            sprintf(str, "%s (%s),%s", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)], rt_lookup[static_cast<int>(context_.curInst_.reg2)]);
            return;

        case AddrMode::MR:
            sprintf(str, "%s (%s)", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)]);
            return;
        case AddrMode::R_MR:
            sprintf(str, "%s %s,(%s)", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)], rt_lookup[static_cast<int>(context_.curInst_.reg2)]);
            return;

        case AddrMode::R_D8:
        case AddrMode::R_A8:
            sprintf(str, "%s %s,$%02X", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)], context_.fetchedData_ & 0xFF);
            return;

        case AddrMode::R_HLI:
            sprintf(str, "%s %s,(%s+)", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)], rt_lookup[static_cast<int>(context_.curInst_.reg2)]);
            return;

        case AddrMode::R_HLD:
            sprintf(str, "%s %s,(%s-)", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)], rt_lookup[static_cast<int>(context_.curInst_.reg2)]);
            return;

        case AddrMode::HLI_R:
            sprintf(str, "%s (%s+),%s", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)], rt_lookup[static_cast<int>(context_.curInst_.reg2)]);
            return;

        case AddrMode::HLD_R:
            sprintf(str, "%s (%s-),%s", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)], rt_lookup[static_cast<int>(context_.curInst_.reg2)]);
            return;

        case AddrMode::A8_R:
            sprintf(str, "%s $%02X,%s", inst_name(context_.curInst_.type), 
                context_.bus_.read(context_.reg_.pc_ - 1), rt_lookup[static_cast<int>(context_.curInst_.reg2)]);

            return;

        case AddrMode::HL_SPR:
            sprintf(str, "%s (%s),SP+%d", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)], context_.fetchedData_ & 0xFF);
            return;

        case AddrMode::D8:
            sprintf(str, "%s $%02X", inst_name(context_.curInst_.type), 
                context_.fetchedData_ & 0xFF);
            return;

        case AddrMode::D16:
            sprintf(str, "%s $%04X", inst_name(context_.curInst_.type), 
                context_.fetchedData_);
            return;

        case AddrMode::MR_D8:
            sprintf(str, "%s (%s),$%02X", inst_name(context_.curInst_.type), 
                rt_lookup[static_cast<int>(context_.curInst_.reg1)], context_.fetchedData_ & 0xFF);
            return;

        case AddrMode::A16_R:
            sprintf(str, "%s ($%04X),%s", inst_name(context_.curInst_.type), 
                context_.fetchedData_, rt_lookup[static_cast<int>(context_.curInst_.reg2)]);
            return;

        default:
            fprintf(stderr, "INVALID AM: %d\n", context_.curInst_.mode);
            NO_IMPL
    }
}


void CPU::fetchInst()
{
    FUNC_TRACE("CPU::fetchInst");
    try {
        uint16_t pc = context_.reg_.pc_;
        context_.curOpcode_ = context_.bus_.read(context_.reg_.pc_++);
        context_.curInst_ = ::std::move(getInstructionByOpCode(context_.curOpcode_));
        /*
        std::cout << context_.reg_.pc_ - 1 << "\t Opcode:" << inst_lookup[static_cast<int>(context_.curInst_.type)] <<
            " " << (int)context_.bus_.read(context_.reg_.pc_ - 1) <<
            " " << (int)context_.bus_.read(context_.reg_.pc_) <<
            " " << (int)context_.bus_.read(context_.reg_.pc_ + 1) << "\t"
            << "A " << (int)context_.reg_.a_ << "\t"
            << "B " << (int)context_.reg_.b_ << "\t"
            << "C " << (int)context_.reg_.c_ << "\t"
            << "D " << (int)context_.reg_.d_ << "\t"
            << "E " << (int)context_.reg_.e_ << "\t"
            << "H " << (int)context_.reg_.h_ << "\t"
            << "L " << (int)context_.reg_.l_ << "\t"
            << "F " << (int)context_.reg_.f_ << "\t"
            << "SP " << context_.reg_.sp_ << "\n";
        */
        
    } catch (std::exception ex) {
        std::cerr << "CPU::fetchInst catches exception: " << ex.what() << "\n";
    } catch (...) {
        std::cerr << "CPU::fetchInst catches exception!\n";
    }
}

void CPU::execute()
{
    FUNC_TRACE("CPU::execute");
    try {
        context_.process();
    } catch (std::exception ex) {
        std::cerr << "CPU::execute catches exception: " << ex.what() << "\n";
    }
    
}

// 执行下一条指令
bool CPU::step()
{
    dbg dbger(context_.bus_);
    if (!context_.halt_) {
        uint16_t pc = context_.reg_.pc_;
        fetchInst();
        context_.fetchData();
        char flags[16];

        char inst[16];
        inst_to_str(inst);
        sprintf(flags, "%c%c%c%c", 
            context_.reg_.f_ & (1 << 7) ? 'Z' : '-',
            context_.reg_.f_ & (1 << 6) ? 'N' : '-',
            context_.reg_.f_ & (1 << 5) ? 'H' : '-',
            context_.reg_.f_ & (1 << 4) ? 'C' : '-'
        );
        /*
        printf("%04X: %-12s (%02X %02X %02X) A: %02X F: %s BC: %02X%02X DE: %02X%02X HL: %02X%02X\n", 
            pc, inst, context_.curOpcode_,
            context_.bus_.read(pc + 1), context_.bus_.read(pc + 2), context_.reg_.a_, flags, context_.reg_.b_, context_.reg_.c_,
            context_.reg_.d_, context_.reg_.e_, context_.reg_.h_, context_.reg_.l_);
        */
        dbger.dbg_update();
        execute();
    } else {

        cycle_.cycle(1);

        if (context_.intFlag_) {
            context_.halt_ = false;
        }

        if (context_.interruptEnabled_) {
            // context_.handleInterrupts();
            context_.enablingIME_ = false;
        }

        if (context_.enablingIME_) {
            context_.interruptEnabled_ = true;
        }
    }
    return true;
}
}