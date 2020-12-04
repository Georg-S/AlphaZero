cmake_minimum_required(VERSION 3.15)
project(AlphaZero)
set(CMAKE_CXX_STANDARD 17)


file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(AlphaZero STATIC ${SOURCES})
target_include_directories(AlphaZero PUBLIC include)
target_link_libraries(AlphaZero "${TORCH_LIBRARIES}")