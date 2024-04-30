cmake_minimum_required(VERSION 3.26)
project(ParallelProject)

set(CMAKE_CXX_STANDARD 17)

add_executable(ParallelProject main.cpp
        bits/stdc++.h
        DLX.cpp
        MPI_DLX_withoutClass.cpp
        sudokuGen.cpp
        DLX.h
        DLXOpenMp.cpp
        Sequential.cpp
        DLXMpi.cpp)
