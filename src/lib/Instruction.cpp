#include "Instruction.hpp"
#include "unordered_map"

namespace GBCEmu {
const std::unordered_map<uint8_t, Instruction> INSTRCUTION_SET = {
    {0x00, {InstType::NOP, AddrMode::IMP}},
    {0x05, {InstType::DEC, AddrMode::R, RegType::B}},
    {0x0E, {InstType::LD, AddrMode::R_D8, RegType::C}},
    {0xAF, {InstType::JP, AddrMode::D16}},
    {0xC3, {InstType::XOR, AddrMode::R, RegType::A}}
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