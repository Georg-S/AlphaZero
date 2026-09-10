cmake_minimum_required(VERSION 3.15)
project(Benchmark)
set(CMAKE_CXX_STANDARD 17)

# Small headless harness used to measure the chess training performance without
# having to drive it through the Qt UI.
add_executable(Benchmark
    "src/Benchmark.cpp"
    )

target_link_libraries(Benchmark AlphaZero Games)

if (MSVC)
    target_compile_options(Benchmark PRIVATE "/MP")
endif (MSVC)
