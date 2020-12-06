cmake_minimum_required(VERSION 3.15)
project(Main)
set(CMAKE_CXX_STANDARD 17)

if	(WIN32)
	list(APPEND CMAKE_PREFIX_PATH $ENV{QT_64_MSVC2017})
endif(WIN32)

find_package(Qt5Widgets REQUIRED)

set(GameHandlingFiles
    "include/GameHandling/ChessHandler.h"
    "include/GameHandling/ConnectFourHandler.h"
    "include/GameHandling/Evaluation.h"
    "include/GameHandling/TicTacToeHandler.h"
    "include/GameHandling/TrainingParameters.h"
    "src/GameHandling/ChessHandler.cpp"
    "src/GameHandling/ConnectFourHandler.cpp"
    "src/GameHandling/Evaluation.cpp"
    "src/GameHandling/TicTacToeHandler.cpp"
    )

set(UIFiles
    "include/UI/MainWindow.h"
    "include/UI/MainMenu.h"

    "src/UI/MainWindow.cpp"
    "src/UI/MainMenu.cpp"
    )

set(UIConnectFourFiles
    "include/UI/ConnectFour/ConnectFourMainMenu.h"
    "include/UI/ConnectFour/ConnectFourMinMaxMenu.h"
    "include/UI/ConnectFour/ConnectFourNeuralNetMenu.h"
    "include/UI/ConnectFour/ConnectFourTraining.h"

    "src/UI/ConnectFour/ConnectFourMainMenu.cpp"
    "src/UI/ConnectFour/ConnectFourMinMaxMenu.cpp"
    "src/UI/ConnectFour/ConnectFourNeuralNetMenu.cpp"
    "src/UI/ConnectFour/ConnectFourTraining.cpp"
    )

set(UITicTacToeFiles
    "include/UI/TicTacToe/TicTacToeMainMenu.h"
    "include/UI/TicTacToe/TicTacToeMinMaxMenu.h"
    "include/UI/TicTacToe/TicTacToeNeuralNetMenu.h"
    "include/UI/TicTacToe/TicTacToeTraining.h"

    "src/UI/TicTacToe/TicTacToeMainMenu.cpp"
    "src/UI/TicTacToe/TicTacToeTraining.cpp"
    "src/UI/TicTacToe/TicTacToeMinMaxMenu.cpp"
    "src/UI/TicTacToe/TicTacToeNeuralNetMenu.cpp"
    )

set(UIChessFiles
    "include/UI/Chess/ChessMainMenu.h"
    "include/UI/Chess/ChessMinMaxMenu.h"
    "include/UI/Chess/ChessNeuralNetMenu.h"
    "include/UI/Chess/ChessTrainingMenu.h"

    "src/UI/Chess/ChessMainMenu.cpp"
    "src/UI/Chess/ChessMinMaxMenu.cpp"
    "src/UI/Chess/ChessNeuralNetMenu.cpp"
    "src/UI/Chess/ChessTrainingMenu.cpp"
    ) 

set(TestFiles
    "src/UI/MainMenu.ui"
    "src/UI/MainWindow.ui"
    "src/UI/MinMax.ui"
    "src/UI/NeuralNet.ui"
    "src/UI/Training.ui"

    "src/UI/Chess/ChessMain.ui"
    "src/UI/ConnectFour/ConnectFourMain.ui"
    "src/UI/TicTacToeMain/TicTacToeMain.ui"
)

set(MainFile "src/main")

source_group("" FILES ${MainFile})
source_group("\\GameHandling" FILES ${GameHandlingFiles})
source_group("\\UI" FILES ${UIFiles})
source_group("\\UI\\ConnectFour" FILES ${UIConnectFourFiles})
source_group("\\UI\\TicTacToe" FILES ${UITicTacToeFiles})
source_group("\\UI\\Chess" FILES ${UIChessFiles})

set(CMAKE_AUTOUIC_SEARCH_PATHS "include/UI")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(Main
    ${GameHandlingFiles} 
    ${UIFiles}
    ${UIConnectFourFiles}
    ${UITicTacToeFiles}
    ${UIChessFiles}
    ${MainFile}
    )

target_include_directories(Main PUBLIC include)
target_link_libraries(Main AlphaZero Games Qt5::Widgets)


add_custom_command(TARGET Main 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E make_directory 
    "$<TARGET_FILE_DIR:Main>/Images")

add_custom_command(TARGET Main
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_SOURCE_DIR}/Images"
                    "$<TARGET_FILE_DIR:Main>/Images")

if (MSVC)
    set_property(TARGET Main PROPERTY WIN32_EXECUTABLE true)

    file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
    add_custom_command(TARGET Main
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${TORCH_DLLS}
                        $<TARGET_FILE_DIR:Main>)

    add_custom_command(TARGET Main
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "$ENV{SDL2_BIN}/SDL2.dll"
                        $<TARGET_FILE_DIR:Main>)

    file(GLOB SDL2_IMAGE_DLLS "$ENV{SDL2IMAGE_BIN}/*.dll")
    add_custom_command(TARGET Main
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${SDL2_IMAGE_DLLS}
                        $<TARGET_FILE_DIR:Main>)
endif (MSVC)
