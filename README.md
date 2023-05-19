This is the implementation of my master's thesis project.

# AlphaZero

This project is split up into several sub-projects

1.  AlphaZero

This contains my implementation of the deep reinforcement learning algorithm "AlphaZero" invented by Silver. et. al.

For an in depth explanation,  look for the sources linked down below.

Libtorch (the C++ version of pytorch) was used to create the neural networks.

2.  Games

This project contains the implemented Games.

Right now the following games are implemented: Tic-Tac-Toe, Connect Four, Chess

3.  Unit Tests

The unit tests for the AlphaZero and Games project.

4.  Main

This project contains the implementation of a very simple User Interface to access the AlphaZero and the Games project.

Here are some examples of the UI. The main menu:

![MainMenu](/readme_files/MainMenu.png)

The connect four main menu:

![ConnectFourMenu](/readme_files/ConnectFourMenu.PNG)

The "Play against mini max menu" for connect four:

![PlayAgainstMiniMax](/readme_files/PlayAgainstMinimax.PNG)

The chess game window:

![Chess](/readme_files/Chess.PNG)

# Installation

## Windows

Tested with the following:
-	Libtorch Cuda 11.1
-	Cuda Toolkit 11.1
-	cuDNN v8.1.1
-	Visual Studio 2019 (Community)
-	Qt 5.14.2
-	Boost 1.82

Install the following:

-	CMake
-	Libtorch
-	Cuda Toolkit	with the apropriate version to Libtorch
-	cuDNN		with the appropriate version to the Cuda Toolkit
-	SDL2
-	SDL2_image
-	Visual Studio 2019 (you need the MSVC)
-	Qt
-	Boost


Set the following Enviroment Variables:
-	QT_64_MSVC2017		   
pointing to for example C:/Qt/Qt5.14.2/5.14.2/msvc2017_64
-	Libtorch_DEBUG		   
pointing to for example C:/Code/libraries/libtorch_debug
-	Libtorch_RELEASE    
pointing to for example C:/Code/libraries/libtorch_release
-	SDL2_INCLUDE         
pointing to for example C:/Code/libraries/SDL2/include
-	SDL2_BIN             
pointing to for example C:/Code/libraries/SDL2/lib/x64
-	SDL2IMAGE_INCLUDE    
pointing to for example C:/Code/libraries/SDL2_image/include
-	SDL2IMAGE_BIN        
pointing to for example C:/Code/libraries/SDL2_image/lib/x64
-	BOOST_ROOT
pointing to for example C:/Libraries/boost_1_82_0


If you want to build in Debug you must make sure that (in the CMakeLists.txt file in the most upper folder):
- the line "list(APPEND CMAKE_PREFIX_PATH $ENV{Libtorch_DEBUG})" is not commented out
- the line "list(APPEND CMAKE_PREFIX_PATH $ENV{Libtorch_RELEASE})" is commented out 

If you want to build in Release you must make sure that:
- the line "list(APPEND CMAKE_PREFIX_PATH $ENV{Libtorch_DEBUG})" is commented out
- the line "list(APPEND CMAKE_PREFIX_PATH $ENV{Libtorch_RELEASE})" is not commented out 


### Building 

Run cmake_build.bat

- Open the solution in the build directory
- Build the solution 

If u want to run the project directly out of Visual Studio, you must set the following:
- Solution Explorer -> Rightclick Main -> Properties -> Debugging -> Working Directory must be set to the folder, where the .exe resides in (it should be in build/Main/Debug or build/Main/Release)

## Linux

Tested with the following:
-	gcc 9.3.0
-	cmake 3.18.4
-	Cuda Toolkit 10.1
-	libtorch cuda 10.2
-	Qt 5.9.7

Install the following:

-	CMake
-	Libtorch
-	Cuda    Toolkit	with the apropriate version to Libtorch
-	cuDNN		with the appropriate version to the Cuda Toolkit
-	SDL2
-	SDL2_image
-	Qt
-	Boost

Set the following Enviroment Variable:
-	LIBTORCH      pointing to your downloaded/installed libtorch folder

Alternatively you could add the path to libtorch to the path variable

### Building (Command line)
-	create build directory
-	run the command: cmake ..
-	run the command: make all


##  Main Sources

* Silver David et. al., 28.01.2016, Mastering the game of Go with deep neural networks and tree search

* Silver David et. al., 19.10.2017, Mastering the game of Go without human knowledge

* Silver David et. al., 5.12.2017, Mastering Chess and Shogi by Self-Play with a General Reinforcement Learning Algorithm

* Lapan Maxim, 2018, Deep Reinforcement Learning Hands-On

* Nair Surag, 29.12.2017, https://web.stanford.edu/~surag/posts/alphazero.html

* Nair Surag, https://github.com/suragnair/alpha-zero-general
