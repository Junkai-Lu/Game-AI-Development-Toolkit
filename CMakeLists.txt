cmake_minimum_required(VERSION 3.2)
project(Paradoxes VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(SRC_LIST
        ./src/gadtlib.h
        ./src/gadtlib.cpp
        ./src/bitboard.hpp
        ./src/gameshell.h
        ./src/gameshell.cpp
        ./src/monte_carlo.hpp
        ./src/mcts.hpp
        ./src/visual_tree.h
        ./src/visual_tree.cpp
		./samples/gadttest.h
        ./samples/gadttest.cpp
        ./samples/gadtmain.cpp
        )
 
ADD_EXECUTABLE(Paradoxes ${SRC_LIST})