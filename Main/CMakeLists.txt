cmake_minimum_required(VERSION 3.15)
project(Main)
set(CMAKE_CXX_STANDARD 17)

if	(WIN32)
	list(APPEND CMAKE_PREFIX_PATH "C:/Qt/Qt5.14.2/5.14.2/msvc2017_64")
endif(WIN32)


find_package(Qt5Widgets REQUIRED)

set(SOURCES
        src/GameHandling/ChessHandler.cpp
        src/GameHandling/ConnectFourHandler.cpp
        src/GameHandling/Evaluation.cpp
        src/GameHandling/TicTacToeHandler.cpp
        src/main.cpp
        src/UI/MainWindow.cpp
        src/UI/MainMenu.cpp
        src/UI/ConnectFour/ConnectFourMainMenu.cpp
        src/UI/ConnectFour/ConnectFourMinMaxMenu.cpp
        src/UI/ConnectFour/ConnectFourNeuralNetMenu.cpp
        src/UI/ConnectFour/ConnectFourTraining.cpp
        src/UI/Chess/ChessMainMenu.cpp
        src/UI/Chess/ChessTrainingMenu.cpp
        src/UI/Chess/ChessMinMaxMenu.cpp
        src/UI/Chess/ChessNeuralNetMenu.cpp
        src/UI/TicTacToe/TicTacToeMainMenu.cpp
        src/UI/TicTacToe/TicTacToeTraining.cpp
        src/UI/TicTacToe/TicTacToeMinMaxMenu.cpp
        src/UI/TicTacToe/TicTacToeNeuralNetMenu.cpp
        )

set(MOC_HEADERS
        include/UI/MainWindow.h
        include/UI/MainMenu.h
        include/UI/ConnectFour/ConnectFourMainMenu.h
        include/UI/ConnectFour/ConnectFourMinMaxMenu.h
        include/UI/ConnectFour/ConnectFourNeuralNetMenu.h
        include/UI/ConnectFour/ConnectFourTraining.h
        include/UI/Chess/ChessMainMenu.h
        include/UI/Chess/ChessTrainingMenu.h
        include/UI/Chess/ChessMinMaxMenu.h
        include/UI/Chess/ChessNeuralNetMenu.h
        include/UI/TicTacToe/TicTacToeMainMenu.h
        include/UI/TicTacToe/TicTacToeTraining.h
        include/UI/TicTacToe/TicTacToeMinMaxMenu.h
        include/UI/TicTacToe/TicTacToeNeuralNetMenu.h
        )


set(CMAKE_AUTOUIC_SEARCH_PATHS src/UI src/UI/ConnectFour src/UI/Chess src/UI/TicTacToe)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(Main ${SOURCES} ${MOC_HEADERS})
target_include_directories(Main PUBLIC include)
target_link_libraries(Main AlphaZero Games Qt5::Widgets)

