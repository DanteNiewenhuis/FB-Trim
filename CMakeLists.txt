cmake_minimum_required(VERSION 3.0)

project(main)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-std=c++14")
set(CMAKE_CXX_FLAGS "-pthread")

set(CMAKE_PREFIX_PATH libtorch)

find_package(Torch REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/executables)

add_executable(
    main
    c++/DataStructures/Graph.cpp 
    c++/DataStructures/AdjacencyMatrix.cpp 
    c++/Algorithms/Kosaraju.cpp 
    c++/Algorithms/FB.cpp
    c++/AI/AINetwork.hpp
    c++/Utils.cpp
    c++/Executables/Main.cpp 
)

add_executable(
    info
    c++/DataStructures/Graph.cpp 
    c++/DataStructures/AdjacencyMatrix.cpp
    c++/Utils.cpp
    c++/Executables/Info.cpp
)

target_link_libraries(main "${TORCH_LIBRARIES}")

set_property(TARGET main PROPERTY CXX_STANDARD 14)