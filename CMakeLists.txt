cmake_minimum_required(VERSION 3.2)
project(General-AI-Development-Toolkit VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(SRC_LIST
        ./src/gadtlib.h
        ./src/gadtlib.cpp
        ./src/bitboard.hpp
        ./src/gameshell.h 
        ./src/gameshell.cpp 
        ./src/mcts.hpp 
        ./src/mcts_new.hpp
        ./src/visual_tree.h
        ./src/visual_tree.cpp
		./sample/gadttest.h 
        ./sample/gadttest.cpp
        ./sample/gadtmain.cpp
        )
 
ADD_EXECUTABLE(Game-AI-Dev-Toolkit ${SRC_LIST})