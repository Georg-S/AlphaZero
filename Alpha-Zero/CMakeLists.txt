cmake_minimum_required(VERSION 3.15)
project(AlphaZero)
set(CMAKE_CXX_STANDARD 17)

set(HEADERS 
	"include/AlphaZero/AlphaZeroTraining.h"
	"include/AlphaZero/Game.h"
	"include/AlphaZero/MonteCarloTreeSearch.h"
	"include/AlphaZero/ReplayElement.h"
	"include/AlphaZero/RingBuffer.h"
	"include/AlphaZero/Training.h"
	)

set(SOURCES
	"src/AlphaZero/AlphaZeroTraining.cpp"
	"src/AlphaZero/MonteCarloTreeSearch.cpp"
	"src/AlphaZero/Training.cpp"
	"src/AlphaZero/NeuralNetworks/DefaultNoResidual.cpp"
)

add_library(AlphaZero STATIC ${SOURCES} ${HEADERS})
target_include_directories(AlphaZero PUBLIC "include")
target_link_libraries(AlphaZero ${TORCH_LIBRARIES})