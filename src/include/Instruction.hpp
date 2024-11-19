#pragma once

#include "Common.hpp"


namespace GBCEmu {

enum class AddrMode {
    R_D16, R_R, MR_R, R_MC, MC_R, R, R_D8, R_MR, R_HLI, R_HLD, HLI_R, HLD_R, R_A8, A8_R, R_HA8, HA8_R, HL_SPR, D16, D8, IMP, D16_R, MR_D8, MR, A16_R, R_A16
};

enum class RegType {
    NONE, A, F, B, C, D, E, H, L, AF, BC, DE, HL, SP, PC
};

enum class InstType {
    NONE = 0, NOP, LD, INC, DEC, RLCA, ADD, RRCA, STOP, RLA, JR, RRA, DAA, CPL, SCF, CCF, HALT, ADC, SUB, SBC, AND, XOR, OR, CP, POP, JP, PUSH, RET, CB, CALL, RETI, LDH, JPHL, DI, EI, RST, ERR,
    RLC, RRC, RL, RR, SLA, SRA, SWAP, SBL, BIT, RES, SET
};
enum class CondType {NONE, NZ, Z, NC, C};


typedef struct {
    InstType type;
    AddrMode mode;
    RegType reg1;
    RegType reg2;
    CondType cond;
    uint8_t param;
} Instruction;

Instruction getInstructionByOpCode(uint8_t opCode);

static bool is16bitReg(RegType reg) {
    return reg == RegType::AF ||  reg == RegType::SP ||  reg == RegType::HL || reg == RegType::AF || reg == RegType::BC || reg == RegType::DE;
}

}