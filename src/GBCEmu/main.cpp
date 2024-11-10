// src/main.cpp
#include <iostream>
#include "Emulator.hpp"

int main(int argc, char* argv[])
{
    GBCEmu::Emulator emulator;
    return emulator.run(argc, argv);
}