// src/main.cpp
#define SDL_MAIN_HANDLED
#include <iostream>
#include "Emulator.hpp"

int main(int argc, char* argv[])
{
    std::cout << "input" << std::endl;
    GBCEmu::Emulator emulator;
    return emulator.run(argc, argv);
}