cmake_minimum_required(VERSION 3.10)
project(GBCEmulator)
set(CMAKE_PREFIX_PATH "D:/qq_Robot_v0.2/SDL2-devel-2.30.9-VC/SDL2-2.30.9/cmake/")

# 查找 SDL2 库（如果使用 SDL2）
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(GBCEmulator main.cpp)

# 链接 SDL2 库（如果使用 SDL2）
target_link_libraries(GBCEmulator PRIVATE GBCEmulator_lib)