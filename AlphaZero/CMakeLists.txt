cmake_minimum_required(VERSION 3.15)
project(AlphaZero)
set(CMAKE_CXX_STANDARD 17)

set(AlphaZeroFiles
	"include/AlphaZeroTraining.h"
	"include/Game.h"
	"include/MonteCarloTreeSearch.h"
	"include/ReplayElement.h"
	"include/RingBuffer.h"
	"include/MultiThreadingNeuralNetManager.h"
	"src/MultiThreadingNeuralNetManager.cpp"
	"src/AlphaZeroTraining.cpp"
	"src/MonteCarloTreeSearch.cpp"
	)

set(NeuralNetworkFiles 
	"include/NeuralNetworks/DefaultNeuralNet.h"
	"include/NeuralNetworks/NeuralNetwork.h"
	"src/NeuralNetworks/DefaultNeuralNet.cpp"
	"include/NeuralNetworks/CutDownAlphaGoZeroNet.h"
	"include/NeuralNetworks/AlphaGoZeroNet.h"
	)

set(NeuralNetworkComponentsFiles
	"include/NeuralNetworks/Components/ResidualBlock.h"
	"include/NeuralNetworks/Components/ConvolutionBlock.h"
	"include/NeuralNetworks/Components/PolicyHead.h"
	"include/NeuralNetworks/Components/ValueHead.h"
)

source_group("\\NeuralNetwork" FILES ${NeuralNetworkFiles})
source_group("\\AlphaZero" FILES ${AlphaZeroFiles})
source_group("\\NeuralNetwork\\Components" FILES ${NeuralNetworkComponentsFiles})


add_library(AlphaZero STATIC 
	${AlphaZeroFiles} 
	${NeuralNetworkFiles}
	${NeuralNetworkComponentsFiles}
	)
target_include_directories(AlphaZero PUBLIC "include")
target_link_libraries(AlphaZero "${TORCH_LIBRARIES}")

if (MSVC)
    target_compile_options(AlphaZero PRIVATE "/MP")
endif (MSVC)