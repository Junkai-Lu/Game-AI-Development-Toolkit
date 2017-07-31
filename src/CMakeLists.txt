cmake_minimum_required(VERSION 3.2)
project(General-AI-Development-Toolkit VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(SRC_LIST
        gadtlib.h
        gadtlib.cpp
        bitboard.hpp
        gadttest.h 
        gadttest.cpp
        gameshell.h 
        gameshell.cpp 
        mcts.hpp 
		mcts_new.hpp
		visual_tree.h
		visual_tree.cpp
        )
 
ADD_EXECUTABLE(GADT ${SRC_LIST})