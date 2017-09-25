cmake_minimum_required(VERSION 3.2)
project(Paradoxes VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXE_LINKER_FLAGS "-pthread")

SET(SRC_LIST
        ./src/gadtlib.h
        ./src/gadtlib.cpp
		./src/game_shell.h
        ./src/game_shell.cpp
        ./src/bitboard.hpp
		./src/gadt_container.hpp
		./src/gadt_memory.hpp
		./src/gadt_log.hpp
        ./src/visual_tree.h
        ./src/visual_tree.cpp
        ./src/gadt_game.hpp
		./src/gadt_algorithm.hpp
        ./src/monte_carlo.hpp
		./src/minimax.hpp
        ./src/mcts.hpp
		./samples/gadt_unittest.h
        ./samples/gadt_unittest.cpp
        ./samples/gadtmain.cpp
		./samples/game/renju.hpp
        )
 
ADD_EXECUTABLE(Paradoxes ${SRC_LIST})