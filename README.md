This is the implementation of my master's thesis project. Currently this project gets refactored to some extend. 

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

##  Main Sources

* Silver David et. al., 28.01.2016, Mastering the game of Go with deep neural networks and tree search

* Silver David et. al., 19.10.2017, Mastering the game of Go without human knowledge

* Silver David et. al., 5.12.2017, Mastering Chess and Shogi by Self-Play with a General Reinforcement Learning Algorithm

* Lapan Maxim, 2018, Deep Reinforcement Learning Hands-On

* Nair Surag, 29.12.2017, https://web.stanford.edu/~surag/posts/alphazero.html

* Nair Surag, https://github.com/suragnair/alpha-zero-general
