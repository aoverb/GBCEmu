#include "Instruction.hpp"
#include "unordered_map"

namespace GBCEmu {
const std::unordered_map<uint8_t, Instruction> INSTRCUTION_SET = {
    {0x00, {InstType::NOP, AddrMode::IMP}},
    {0x01, {InstType::LD, AddrMode::R_D16, RegType::BC}},
    {0x02, {InstType::LD, AddrMode::MR_R, RegType::BC, RegType::A}},
    {0x03, {InstType::INC, AddrMode::R, RegType::BC}},
    {0x04, {InstType::INC, AddrMode::R, RegType::B}},
    {0x05, {InstType::DEC, AddrMode::R, RegType::B}},
    {0x06, {InstType::LD, AddrMode::R_D8, RegType::B}},
    {0x08, {InstType::LD, AddrMode::A16_R, RegType::NONE, RegType::SP}},
    {0x0A, {InstType::LD, AddrMode::R_MR, RegType::A, RegType::BC}},
    {0x0B, {InstType::DEC, AddrMode::R, RegType::BC}},
    {0x0C, {InstType::INC, AddrMode::R, RegType::C}},
    {0x0D, {InstType::DEC, AddrMode::R, RegType::C}},
    {0x0E, {InstType::LD, AddrMode::R_D8, RegType::C}},

    {0x11, {InstType::LD, AddrMode::R_D16, RegType::DE}},
    {0x12, {InstType::LD, AddrMode::MR_R, RegType::DE, RegType::A}},
    {0x13, {InstType::INC, AddrMode::R, RegType::DE}},
    {0x14, {InstType::INC, AddrMode::R, RegType::D}},
    {0x15, {InstType::DEC, AddrMode::R, RegType::D}},
    {0x16, {InstType::LD, AddrMode::R_D8, RegType::D}},
    {0x18, {InstType::JR, AddrMode::D8}},
    {0x1A, {InstType::LD, AddrMode::R_MR, RegType::A, RegType::DE}},
    {0x1B, {InstType::DEC, AddrMode::R, RegType::DE}},
    {0x1C, {InstType::INC, AddrMode::R, RegType::E}},
    {0x1D, {InstType::DEC, AddrMode::R, RegType::E}},
    {0x1E, {InstType::LD, AddrMode::R_D8, RegType::E}},

    {0x20, {InstType::JR, AddrMode::D8, RegType::NONE, RegType::NONE, CondType::NZ}},
    {0x21, {InstType::LD, AddrMode::R_D16, RegType::HL}},
    {0x22, {InstType::LD, AddrMode::HLI_R, RegType::HL, RegType::A}},
    {0x23, {InstType::INC, AddrMode::R, RegType::HL}},
    {0x24, {InstType::INC, AddrMode::R, RegType::H}},
    {0x25, {InstType::DEC, AddrMode::R, RegType::H}},
    {0x26, {InstType::LD, AddrMode::R_D8, RegType::H}},
    {0x28, {InstType::JR, AddrMode::D8, RegType::NONE, RegType::NONE, CondType::Z}},
    {0x2A, {InstType::LD, AddrMode::R_HLI, RegType::A, RegType::HL}},
    {0x2B, {InstType::DEC, AddrMode::R, RegType::HL}},
    {0x2C, {InstType::INC, AddrMode::R, RegType::L}},
    {0x2D, {InstType::DEC, AddrMode::R, RegType::L}},
    {0x2E, {InstType::LD, AddrMode::R_D8, RegType::L}},

    {0x30, {InstType::JR, AddrMode::D8, RegType::NONE, RegType::NONE, CondType::NC}},
    {0x31, {InstType::LD, AddrMode::R_D16, RegType::SP}},
    {0x32, {InstType::LD, AddrMode::HLD_R, RegType::HL, RegType::A}},
    {0x33, {InstType::INC, AddrMode::R, RegType::SP}},
    {0x34, {InstType::INC, AddrMode::MR, RegType::HL}},
    {0x35, {InstType::DEC, AddrMode::MR, RegType::HL}},
    {0x36, {InstType::LD, AddrMode::R_D8, RegType::HL}},
    {0x38, {InstType::JR, AddrMode::D8, RegType::NONE, RegType::NONE, CondType::C}},
    {0x3A, {InstType::LD, AddrMode::R_HLD, RegType::A, RegType::HL}},
    {0x3B, {InstType::DEC, AddrMode::R, RegType::SP}},
    {0x3C, {InstType::INC, AddrMode::R, RegType::A}},
    {0x3D, {InstType::DEC, AddrMode::R, RegType::A}},
    {0x3E, {InstType::LD, AddrMode::R_D8, RegType::A}},

    {0x40, {InstType::LD, AddrMode::R_R, RegType::B, RegType::B}},
    {0x41, {InstType::LD, AddrMode::R_R, RegType::B, RegType::C}},
    {0x42, {InstType::LD, AddrMode::R_R, RegType::B, RegType::D}},
    {0x43, {InstType::LD, AddrMode::R_R, RegType::B, RegType::E}},
    {0x44, {InstType::LD, AddrMode::R_R, RegType::B, RegType::H}},
    {0x45, {InstType::LD, AddrMode::R_R, RegType::B, RegType::L}},
    {0x46, {InstType::LD, AddrMode::R_MR, RegType::B, RegType::HL}},
    {0x47, {InstType::LD, AddrMode::R_R, RegType::B, RegType::A}},
    {0x48, {InstType::LD, AddrMode::R_R, RegType::C, RegType::B}},
    {0x49, {InstType::LD, AddrMode::R_R, RegType::C, RegType::C}},
    {0x4A, {InstType::LD, AddrMode::R_R, RegType::C, RegType::D}},
    {0x4B, {InstType::LD, AddrMode::R_R, RegType::C, RegType::E}},
    {0x4C, {InstType::LD, AddrMode::R_R, RegType::C, RegType::H}},
    {0x4D, {InstType::LD, AddrMode::R_R, RegType::C, RegType::L}},
    {0x4E, {InstType::LD, AddrMode::R_MR, RegType::C, RegType::HL}},
    {0x4F, {InstType::LD, AddrMode::R_R, RegType::C, RegType::A}},

    {0x50, {InstType::LD, AddrMode::R_R, RegType::D, RegType::B}},
    {0x51, {InstType::LD, AddrMode::R_R, RegType::D, RegType::C}},
    {0x52, {InstType::LD, AddrMode::R_R, RegType::D, RegType::D}},
    {0x53, {InstType::LD, AddrMode::R_R, RegType::D, RegType::E}},
    {0x54, {InstType::LD, AddrMode::R_R, RegType::D, RegType::H}},
    {0x55, {InstType::LD, AddrMode::R_R, RegType::D, RegType::L}},
    {0x56, {InstType::LD, AddrMode::R_MR, RegType::D, RegType::HL}},
    {0x57, {InstType::LD, AddrMode::R_R, RegType::D, RegType::A}},
    {0x58, {InstType::LD, AddrMode::R_R, RegType::E, RegType::B}},
    {0x59, {InstType::LD, AddrMode::R_R, RegType::E, RegType::C}},
    {0x5A, {InstType::LD, AddrMode::R_R, RegType::E, RegType::D}},
    {0x5B, {InstType::LD, AddrMode::R_R, RegType::E, RegType::E}},
    {0x5C, {InstType::LD, AddrMode::R_R, RegType::E, RegType::H}},
    {0x5D, {InstType::LD, AddrMode::R_R, RegType::E, RegType::L}},
    {0x5E, {InstType::LD, AddrMode::R_MR, RegType::E, RegType::HL}},
    {0x5F, {InstType::LD, AddrMode::R_R, RegType::E, RegType::A}},

    {0x60, {InstType::LD, AddrMode::R_R, RegType::H, RegType::B}},
    {0x61, {InstType::LD, AddrMode::R_R, RegType::H, RegType::C}},
    {0x62, {InstType::LD, AddrMode::R_R, RegType::H, RegType::D}},
    {0x63, {InstType::LD, AddrMode::R_R, RegType::H, RegType::E}},
    {0x64, {InstType::LD, AddrMode::R_R, RegType::H, RegType::H}},
    {0x65, {InstType::LD, AddrMode::R_R, RegType::H, RegType::L}},
    {0x66, {InstType::LD, AddrMode::R_MR, RegType::H, RegType::HL}},
    {0x67, {InstType::LD, AddrMode::R_R, RegType::H, RegType::A}},
    {0x68, {InstType::LD, AddrMode::R_R, RegType::L, RegType::B}},
    {0x69, {InstType::LD, AddrMode::R_R, RegType::L, RegType::C}},
    {0x6A, {InstType::LD, AddrMode::R_R, RegType::L, RegType::D}},
    {0x6B, {InstType::LD, AddrMode::R_R, RegType::L, RegType::E}},
    {0x6C, {InstType::LD, AddrMode::R_R, RegType::L, RegType::H}},
    {0x6D, {InstType::LD, AddrMode::R_R, RegType::L, RegType::L}},
    {0x6E, {InstType::LD, AddrMode::R_MR, RegType::L, RegType::HL}},
    {0x6F, {InstType::LD, AddrMode::R_R, RegType::L, RegType::A}},

    {0x70, {InstType::LD, AddrMode::MR_R, RegType::HL, RegType::B}},
    {0x71, {InstType::LD, AddrMode::MR_R, RegType::HL, RegType::C}},
    {0x72, {InstType::LD, AddrMode::MR_R, RegType::HL, RegType::D}},
    {0x73, {InstType::LD, AddrMode::MR_R, RegType::HL, RegType::E}},
    {0x74, {InstType::LD, AddrMode::MR_R, RegType::HL, RegType::H}},
    {0x75, {InstType::LD, AddrMode::MR_R, RegType::HL, RegType::L}},
    {0x76, {InstType::HALT}},
    {0x77, {InstType::LD, AddrMode::R_R, RegType::H, RegType::A}},
    {0x78, {InstType::LD, AddrMode::R_R, RegType::A, RegType::B}},
    {0x79, {InstType::LD, AddrMode::R_R, RegType::A, RegType::C}},
    {0x7A, {InstType::LD, AddrMode::R_R, RegType::A, RegType::D}},
    {0x7B, {InstType::LD, AddrMode::R_R, RegType::A, RegType::E}},
    {0x7C, {InstType::LD, AddrMode::R_R, RegType::A, RegType::H}},
    {0x7D, {InstType::LD, AddrMode::R_R, RegType::A, RegType::L}},
    {0x7E, {InstType::LD, AddrMode::R_MR, RegType::A, RegType::HL}},
    {0x7F, {InstType::LD, AddrMode::R_R, RegType::A, RegType::A}},

    {0xAF, {InstType::XOR, AddrMode::R, RegType::A}},

    {0xC0, {InstType::RET, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::NZ}},
    {0xC1, {InstType::POP, AddrMode::IMP, RegType::BC}},
    {0xC2, {InstType::JP, AddrMode::D16, RegType::NONE, RegType::NONE, CondType::NZ}},
    {0xC3, {InstType::JP, AddrMode::D16}},
    {0xC4, {InstType::CALL, AddrMode::D16, RegType::NONE, RegType::NONE, CondType::NZ}},
    {0xC5, {InstType::PUSH, AddrMode::IMP, RegType::BC}},
    {0xC7, {InstType::RST, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::NONE, 0x00}},
    {0xC8, {InstType::RET, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::Z}},
    {0xC9, {InstType::RET, AddrMode::IMP}},
    {0xCA, {InstType::JP, AddrMode::D16, RegType::NONE, RegType::NONE, CondType::Z}},
    {0xCC, {InstType::CALL, AddrMode::D16, RegType::NONE, RegType::NONE, CondType::Z}},
    {0xCD, {InstType::CALL, AddrMode::D16}},
    {0xCF, {InstType::RST, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::NONE, 0x08}},

    {0xD0, {InstType::RET, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::NC}},
    {0xD1, {InstType::POP, AddrMode::IMP, RegType::DE}},
    {0xD2, {InstType::JP, AddrMode::D16, RegType::NONE, RegType::NONE, CondType::NC}},
    {0xD4, {InstType::CALL, AddrMode::D16, RegType::NONE, RegType::NONE, CondType::NC}},
    {0xD5, {InstType::PUSH, AddrMode::IMP, RegType::DE}},
    {0xD7, {InstType::RST, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::NONE, 0x10}},
    {0xD8, {InstType::RET, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::C}},
    {0xD9, {InstType::RETI, AddrMode::IMP}},
    {0xDA, {InstType::JP, AddrMode::D16, RegType::NONE, RegType::NONE, CondType::C}},
    {0xDC, {InstType::CALL, AddrMode::D16, RegType::NONE, RegType::NONE, CondType::C}},
    {0xDF, {InstType::RST, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::NONE, 0x18}},

    {0xE0, {InstType::LD, AddrMode::HA8_R, RegType::NONE, RegType::A}},
    {0xE1, {InstType::POP, AddrMode::IMP, RegType::HL}},
    {0xE2, {InstType::LD, AddrMode::MC_R, RegType::C, RegType::A}},
    {0xE5, {InstType::PUSH, AddrMode::IMP, RegType::HL}},
    {0xE7, {InstType::RST, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::NONE, 0x20}},
    {0xE9, {InstType::JP, AddrMode::MR, RegType::HL}},
    {0xEA, {InstType::LD, AddrMode::A16_R, RegType::NONE, RegType::A}},
    {0xEF, {InstType::RST, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::NONE, 0x28}},

    {0xF0, {InstType::LD, AddrMode::R_HA8, RegType::A}},
    {0xF1, {InstType::POP, AddrMode::IMP, RegType::AF}},
    {0xF2, {InstType::LD, AddrMode::R_MC, RegType::A, RegType::C}},
    {0xF3, {InstType::DI, AddrMode::IMP}},
    {0xF5, {InstType::PUSH, AddrMode::IMP, RegType::AF}},
    {0xF7, {InstType::RST, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::NONE, 0x30}},
    {0xFA, {InstType::LD, AddrMode::R_A16, RegType::A}},
    {0xF8, {InstType::LD, AddrMode::R_R, RegType::HL, RegType::SP}},
    {0xF9, {InstType::LD, AddrMode::R_R, RegType::SP, RegType::HL}},
    {0xFF, {InstType::RST, AddrMode::IMP, RegType::NONE, RegType::NONE, CondType::NONE, 0x38}}
};

Instruction getInstructionByOpCode(uint8_t opCode) {
    try {
        return INSTRCUTION_SET.at(opCode);
    } catch(...) {
        std::cerr << "Invalid opCode: " << std::hex << (int)opCode << "!\n";
        exit(-7);
    }
}

}