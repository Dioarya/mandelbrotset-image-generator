cmake_minimum_required(VERSION 3.29.2)
project(MIG C CXX)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE sources src/*.hpp src/*.h src/*.cpp src/*.c)

add_executable(MIG ${sources})
target_link_libraries(MIG PRIVATE gomp)
target_compile_options(MIG PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror -Wextra -Wshadow -Wpedantic -Weffc++ -m64 -std=c++23 -march=native -Ofast -fopenmp -mavx512f>
)