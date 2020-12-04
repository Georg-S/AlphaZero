cmake_minimum_required(VERSION 3.15)
project(Games)
set(CMAKE_CXX_STANDARD 17)


file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(Games STATIC ${SOURCES})


if (UNIX)
	INCLUDE(FindPkgConfig)
	PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
	PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
endif(UNIX)

if	(WIN32)
	set(SDL2_INCLUDE_DIRS "C:/Users/Georg/Documents/Code/libraries/SDL2/include")
	set(SDL2_LIBRARIES "C:/Users/Georg/Documents/Code/libraries/SDL2/lib/x64/SDL2.lib")
	set(SDL2IMAGE_INCLUDE_DIRS "C:/Users/Georg/Documents/Code/libraries/SDL2_image/include")
	set(SDL2IMAGE_LIBRARIES "C:/Users/Georg/Documents/Code/libraries/SDL2_image/lib/x64/SDL2_image.lib")
endif(WIN32)


target_include_directories(Games PUBLIC include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
target_link_libraries(Games AlphaZero ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})