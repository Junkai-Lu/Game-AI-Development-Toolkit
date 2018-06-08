cmake_minimum_required(VERSION 3.2)
project(Paradoxes VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXE_LINKER_FLAGS "-pthread")

SET(SRC_LIST
		./src/gadt_config.h
        ./src/gadtlib.h
        ./src/gadtlib.cpp
		./src/gadt_filesystem.h
		./src/gadt_filesystem.cpp
		./src/game_shell.h
        ./src/game_shell.cpp
		./src/visual_tree.h
        ./src/visual_tree.cpp
        ./src/bitboard.hpp
		./src/gadt_container.hpp
		./src/gadt_game.hpp
		./src/gadt_log.hpp
		./src/gadt_log.cpp
		./src/gadt_table.h
		./src/gadt_table.cpp
		./src/gadt_memory.hpp
		./src/gadt_algorithm.hpp
        ./src/monte_carlo.hpp
		./src/minimax.hpp
        ./src/mcts.hpp
		./src/parallel_mcts.hpp
		./examples/gadt_unittest.h
        ./examples/gadt_unittest.cpp
        ./examples/gadtmain.cpp
		./examples/games/renju.h
		./examples/games/renju.cpp
		./examples/games/ewn.h
		./examples/games/ewn.cpp
		./examples/games/breakthrough.h
		./examples/games/breakthrough.cpp
		./lib/json11/json11.hpp
		./lib/json11/json11.cpp
        )
 
ADD_EXECUTABLE(Paradoxes.sh ${SRC_LIST})