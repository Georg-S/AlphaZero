cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
project(DeepReinforcementLearning)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# The training loop is dominated by CPU work (MCTS tree search + chess move generation).
# Building without optimization (empty build type or Debug) makes it roughly an order of
# magnitude slower, so default to an optimized Release build unless the user asked for
# something else explicitly.
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    message(STATUS "No build type specified, defaulting to Release for training performance")
endif()

# Enable CPU specific optimizations (AVX2/FMA/...) for the local machine. This significantly
# speeds up the CPU bound parts. Disable with -DALZ_NATIVE_OPTIMIZATIONS=OFF if the produced
# binary has to run on a different machine.
option(ALZ_NATIVE_OPTIMIZATIONS "Compile with -march=native" ON)
if (ALZ_NATIVE_OPTIMIZATIONS AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang"))
    add_compile_options($<$<CONFIG:Release>:-march=native>)
    add_compile_options($<$<CONFIG:RelWithDebInfo>:-march=native>)
endif()

if (UNIX)
	list(APPEND CMAKE_PREFIX_PATH $ENV{LIBTORCH})
endif(UNIX)

if	(WIN32)
    list(APPEND CMAKE_PREFIX_PATH $ENV{QT_MSVC_64})
	# list(APPEND CMAKE_PREFIX_PATH $ENV{Libtorch_DEBUG})
	list(APPEND CMAKE_PREFIX_PATH $ENV{Libtorch_RELEASE})

    add_compile_options (/permissive-)
endif(WIN32)

find_package(Torch REQUIRED)

add_subdirectory(AlphaZero)
add_subdirectory(Games)
add_subdirectory(Unit_Tests)
add_subdirectory(Benchmark)
add_subdirectory(Main)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Main)