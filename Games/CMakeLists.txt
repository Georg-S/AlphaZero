cmake_minimum_required(VERSION 3.15)
project(Games)
set(CMAKE_CXX_STANDARD 17)


set(OtherFiles
	"include/Other/Ai.h"
	"include/Other/NeuralNetAi.h"
	"include/Other/Utility.h"
	"src/Other/NeuralNetAi.cpp"
	"src/Other/Utility.cpp"
	)
	
set(ChessEngine
	"include/Chess/Engine/BitBoard.h"
	"include/Chess/Engine/ChessEngine.h"
	"include/Chess/Engine/MoveGenerator.h"
	"include/Chess/Engine/NegamaxAI.h"
	"include/Chess/Engine/Utility.h"
	"include/Chess/Engine/ValueTables.h"
	"src/Chess/Engine/BitBoard.cpp"
	"src/Chess/Engine/ChessEngine.cpp"
	"src/Chess/Engine/MoveGenerator.cpp"
	"src/Chess/Engine/NegamaxAI.cpp"
	"src/Chess/Engine/Utility.cpp"
	)

set(ChessFiles
	"include/Chess/ReducedChessAdapter.h"
	"include/Chess/RenderInformation.h"
	"include/Chess/Chess.h"
	"include/Chess/Constant.h"
	"include/Chess/Renderer.h"
	"include/Chess/RenderInformation.h"
	"include/Chess/ChessAiAdapter.h"
	"src/Chess/ReducedChessAdapter.cpp"
	"src/Chess/Chess.cpp"
	"src/Chess/Renderer.cpp"
	"src/Chess/ChessAiAdapter.cpp"
	)

set(ConnectFourFiles
	"include/ConnectFour/ConnectFour.h"
	"include/ConnectFour/ConnectFourAdapter.h"
	"include/ConnectFour/GameLogic.h"
	"include/ConnectFour/NegaMaxAi.h"
	"include/ConnectFour/Renderer.h"
	"src/ConnectFour/ConnectFour.cpp"
	"src/ConnectFour/ConnectFourAdapter.cpp"
	"src/ConnectFour/GameLogic.cpp"
	"src/ConnectFour/NegaMaxAi.cpp"
	"src/ConnectFour/Renderer.cpp"
)

set(SDLFiles
	"include/SDL/Mouse.h"
	"include/SDL/SDLHandler.h"
	"src/SDL/Mouse.cpp"
	"src/SDL/SDLHandler.cpp"
)

set(TicTacToeFiles
	"include/TicTacToe/GameLogic.h"
	"include/TicTacToe/MiniMaxAi.h"
	"include/TicTacToe/Renderer.h"
	"include/TicTacToe/TicTacToe.h"
	"include/TicTacToe/TicTacToeAdapter.h"
	"src/TicTacToe/GameLogic.cpp"
	"src/TicTacToe/MiniMaxAi.cpp"
	"src/TicTacToe/Renderer.cpp"
	"src/TicTacToe/TicTacToe.cpp"
	"src/TicTacToe/TicTacToeAdapter.cpp"
)

source_group("\\Other" FILES ${OtherFiles})
source_group("\\Chess" FILES ${ChessFiles})
source_group("\\Chess\\Engine" FILES ${ChessEngine})
source_group("\\ConnectFour" FILES ${ConnectFourFiles})
source_group("\\SDL" FILES ${SDLFiles})
source_group("\\TicTacToe" FILES ${TicTacToeFiles})


add_library(Games STATIC 
	${OtherFiles}
	${ChessEngine}
	${ChessFiles}
	${ConnectFourFiles}
	${SDLFiles}
	${TicTacToeFiles}
	)


if (UNIX)
	INCLUDE(FindPkgConfig)
	PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
	PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
endif(UNIX)

if (MSVC)
    target_compile_options(Games PRIVATE "/MP")
endif (MSVC)

if	(WIN32)
	set(SDL2_INCLUDE_DIRS $ENV{SDL2_INCLUDE})
	set(SDL2_LIBRARIES "$ENV{SDL2_BIN}/SDL2.lib")
	set(SDL2IMAGE_INCLUDE_DIRS $ENV{SDL2IMAGE_INCLUDE})
	set(SDL2IMAGE_LIBRARIES "$ENV{SDL2IMAGE_BIN}/SDL2_image.lib")
endif(WIN32)


target_include_directories(Games PUBLIC include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
target_link_libraries(Games AlphaZero ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
