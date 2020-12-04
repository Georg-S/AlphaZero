project(Unit_Tests)
add_subdirectory(lib)


add_executable(Unit_Tests_run Test_NeuralNet.cpp Test_TicTacToeAdapter.cpp Test_RingBuffer.cpp Test_Random.cpp Test_Training.cpp Test_MonteCarloTreeSearch.cpp Test_ConnectFourAdapter.cpp Test_ReducedChessAdapter.cpp Test_Chesscore.cpp)
target_include_directories(Unit_Tests_run PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(Unit_Tests_run gtest gtest_main AlphaZero Games)