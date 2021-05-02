cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
project(DeepReinforcementLearning)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (UNIX)
	list(APPEND CMAKE_PREFIX_PATH $ENV{LIBTORCH})
endif(UNIX)
if	(WIN32)
    list(APPEND CMAKE_PREFIX_PATH $ENV{QT_64_MSVC2017})
#	list(APPEND CMAKE_PREFIX_PATH $ENV{Libtorch_DEBUG})
	list(APPEND CMAKE_PREFIX_PATH $ENV{Libtorch_RELEASE})

    add_compile_options (/permissive-)
endif(WIN32)

find_package(Torch REQUIRED)

add_subdirectory(AlphaZero)
add_subdirectory(Games)
add_subdirectory(Unit_Tests)
add_subdirectory(Main)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Main)