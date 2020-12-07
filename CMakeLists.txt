cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
project(DeepReinforcementLearning)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (UNIX)
	list(APPEND CMAKE_PREFIX_PATH "/home/georg/Documents/Code/Libraries/libtorch")
endif(UNIX)
if	(WIN32)
    list(APPEND CMAKE_PREFIX_PATH $ENV{QT_64_MSVC2017})
	list(APPEND CMAKE_PREFIX_PATH $ENV{Libtorch_DEBUG})
#	list(APPEND CMAKE_PREFIX_PATH $ENV{Libtorch_RELEASE})
endif(WIN32)

find_package(Torch REQUIRED)
find_package(OpenMP REQUIRED)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


add_subdirectory(AlphaZero)
add_subdirectory(Games)
add_subdirectory(Unit_Tests)
add_subdirectory(Main)