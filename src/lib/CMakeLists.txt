cmake_minimum_required(VERSION 3.10)
project(GBCEmulator_lib)
set(CMAKE_PREFIX_PATH "D:/SDL2-2.30.9/x86_64-w64-mingw32/lib/cmake/SDL2/")

# 查找 SDL2 库（如果使用 SDL2）
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(D:/SDL2-2.30.9/x86_64-w64-mingw32/lib)

add_library(GBCEmulator_lib APU.cpp Bus.cpp CPU.cpp CPUContext.cpp CPURegister.cpp Cycle.cpp RTC.cpp DMAContext.cpp dbg.cpp Emulator.cpp Common.cpp Gamepad.cpp RAM.cpp Cartridge.cpp Interrupt.cpp IO.cpp LCD.cpp DMA.cpp PPU.cpp Timer.cpp UI.cpp Instruction.cpp)

# 链接 SDL2 库（如果使用 SDL2）
target_link_libraries(GBCEmulator_lib SDL2main SDL2)