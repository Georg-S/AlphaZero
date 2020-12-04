project(Unit_Tests)
add_subdirectory(lib)


add_executable(Unit_Tests_run Test_NeuralNet.cpp Test_TicTacToeAdapter.cpp Test_RingBuffer.cpp Test_Random.cpp Test_Training.cpp Test_MonteCarloTreeSearch.cpp Test_ConnectFourAdapter.cpp Test_ReducedChessAdapter.cpp Test_Chesscore.cpp)
target_include_directories(Unit_Tests_run PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(Unit_Tests_run gtest gtest_main AlphaZero Games)

if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET Unit_Tests_run
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:Unit_Tests_run>)
endif (MSVC)