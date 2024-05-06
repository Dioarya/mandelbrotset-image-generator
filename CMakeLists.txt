cmake_minimum_required(VERSION 3.29.2)
project(MIG C CXX)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE sources src/*.cpp src/*.c src/*.hpp src/*.h)

add_executable(MIG ${sources})
target_compile_options(MIG PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Ofast -march=native>
)