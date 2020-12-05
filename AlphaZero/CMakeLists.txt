cmake_minimum_required(VERSION 3.15)
project(AlphaZero)
set(CMAKE_CXX_STANDARD 17)

set(AlphaZeroFiles
	"include/AlphaZeroTraining.h"
	"include/Game.h"
	"include/MonteCarloTreeSearch.h"
	"include/ReplayElement.h"
	"include/RingBuffer.h"
	"include/Training.h"
	"src/AlphaZeroTraining.cpp"
	"src/MonteCarloTreeSearch.cpp"
	"src/Training.cpp"
	)

set(NeuralNetworkFiles 
	"include/NeuralNetworks/DefaultNoResidual.h"
	"include/NeuralNetworks/NeuralNetwork.h"
	"include/NeuralNetworks/NoResidualNNImpl.h"
	"src/NeuralNetworks/DefaultNoResidual.cpp"
	)

source_group("\\NeuralNetwork" FILES ${NeuralNetworkFiles})
source_group("\\AlphaZero" FILES ${AlphaZeroFiles})

add_library(AlphaZero STATIC ${AlphaZeroFiles} ${NeuralNetworkFiles})
target_include_directories(AlphaZero PUBLIC "include")
target_link_libraries(AlphaZero ${TORCH_LIBRARIES})