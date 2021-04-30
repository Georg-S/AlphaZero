cmake_minimum_required(VERSION 3.15)
project(Games)
set(CMAKE_CXX_STANDARD 17)


set(AIFiles
	"include/AI/Ai.h"
	"include/AI/NeuralNetAi.h"
	"src/AI/NeuralNetAi.cpp"
	)

set(ChessFiles
	"include/Chess/Board.h"
	"include/Chess/Chess.h"
	"include/Chess/GameLogic.h"
	"include/Chess/MiniMaxAi.h"
	"include/Chess/Move.h"
	"include/Chess/PieceColor.h"
	"include/Chess/ReducedChessAdapter.h"
	"include/Chess/Renderer.h"
	"include/Chess/RenderInformation.h"
	"src/Chess/Chess.cpp"
	"src/Chess/GameLogic.cpp"
	"src/Chess/MiniMaxAi.cpp"
	"src/Chess/ReducedChessAdapter.cpp"
	"src/Chess/Renderer.cpp"
	)

set(ChessPiecesFiles
	"include/Chess/Pieces/Bishop.h"
	"include/Chess/Pieces/King.h"
	"include/Chess/Pieces/Knight.h"
	"include/Chess/Pieces/Pawn.h"
	"include/Chess/Pieces/Piece.h"
	"include/Chess/Pieces/Queen.h"
	"include/Chess/Pieces/Rook.h"
	"src/Chess/Pieces/Bishop.cpp"
	"src/Chess/Pieces/King.cpp"
	"src/Chess/Pieces/Knight.cpp"
	"src/Chess/Pieces/Pawn.cpp"
	"src/Chess/Pieces/Piece.cpp"
	"src/Chess/Pieces/Queen.cpp"
	"src/Chess/Pieces/Rook.cpp"
)

set(ConnectFourFiles
	"include/ConnectFour/Board.h"
	"include/ConnectFour/ConnectFour.h"
	"include/ConnectFour/ConnectFourAdapter.h"
	"include/ConnectFour/GameLogic.h"
	"include/ConnectFour/MiniMaxAi.h"
	"include/ConnectFour/PlayerColor.h"
	"include/ConnectFour/Renderer.h"
	"src/ConnectFour/ConnectFour.cpp"
	"src/ConnectFour/ConnectFourAdapter.cpp"
	"src/ConnectFour/GameLogic.cpp"
	"src/ConnectFour/MiniMaxAi.cpp"
	"src/ConnectFour/Renderer.cpp"
)

set(SDLFiles
	"include/SDL/Mouse.h"
	"include/SDL/MouseInformation.h"
	"include/SDL/SDLHandler.h"
	"src/SDL/Mouse.cpp"
	"src/SDL/SDLHandler.cpp"
)

set(TicTacToeFiles
	"include/TicTacToe/Board.h"
	"include/TicTacToe/GameLogic.h"
	"include/TicTacToe/MiniMaxAi.h"
	"include/TicTacToe/Move.h"
	"include/TicTacToe/Renderer.h"
	"include/TicTacToe/TicTacToe.h"
	"include/TicTacToe/TicTacToeAdapter.h"
	"src/TicTacToe/GameLogic.cpp"
	"src/TicTacToe/MiniMaxAi.cpp"
	"src/TicTacToe/Renderer.cpp"
	"src/TicTacToe/TicTacToe.cpp"
	"src/TicTacToe/TicTacToeAdapter.cpp"
)

source_group("\\AI" FILES ${AIFiles})
source_group("\\Chess" FILES ${ChessFiles})
source_group("\\Chess\\Pieces" FILES ${ChessPiecesFiles})
source_group("\\ConnectFour" FILES ${ConnectFourFiles})
source_group("\\SDL" FILES ${SDLFiles})
source_group("\\TicTacToe" FILES ${TicTacToeFiles})


add_library(Games STATIC 
	${AIFiles}
	${ChessFiles}
	${ChessPiecesFiles}
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
