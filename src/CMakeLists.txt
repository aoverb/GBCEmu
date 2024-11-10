cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)

# 添加 include 目录到全局包含路径
include_directories(${CMAKE_SOURCE_DIR}/src/include)

add_subdirectory(lib)
add_subdirectory(GBCEmu)