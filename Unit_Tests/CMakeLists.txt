project(Unit_Tests)
add_subdirectory(lib)
enable_testing()
set_target_properties(gmock gmock_main gtest gtest_main PROPERTIES FOLDER GoogleTestTargets)

set(TESTSOURCES 
    "Test_Utility.h"
    "TestConfig.h"
    "Test_Utility.cpp"
    "Test_NeuralNet.cpp"
    "Test_TicTacToeAdapter.cpp"
    "Test_RingBuffer.cpp"
    "Test_Miscellaneous.cpp"
    "Test_MCTS_TicTacToe.cpp"
    "Test_MCTS_ConnectFour.cpp"
    "Test_MCTS_Chess.cpp"
    "Test_Training.cpp"
    "Test_ConnectFourAdapter.cpp"
    "Test_ChessAdapter.cpp"
    )

add_executable(Unit_Tests ${TESTSOURCES})
target_link_libraries(Unit_Tests gtest gtest_main AlphaZero Games)
add_test(NAME Unit_Tests COMMAND Unit_Tests)

#   create image folder and copy files into it
add_custom_command(TARGET Unit_Tests 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E make_directory 
    "$<TARGET_FILE_DIR:Unit_Tests>/Images")

add_custom_command(TARGET Unit_Tests
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_SOURCE_DIR}/Images"
                    "$<TARGET_FILE_DIR:Unit_Tests>/Images")

if (MSVC)
  target_compile_options(Unit_Tests PRIVATE "/MP")

  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET Unit_Tests
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:Unit_Tests>)


    add_custom_command(TARGET Unit_Tests
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "$ENV{SDL2_BIN}/SDL2.dll"
                        $<TARGET_FILE_DIR:Unit_Tests>)

    file(GLOB SDL2_IMAGE_DLLS "$ENV{SDL2IMAGE_BIN}/*.dll")
    add_custom_command(TARGET Unit_Tests
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${SDL2_IMAGE_DLLS}
                        $<TARGET_FILE_DIR:Unit_Tests>)
endif (MSVC)