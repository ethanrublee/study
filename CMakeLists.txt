cmake_minimum_required(VERSION 2.8)
project(study_study)
add_library(registry SHARED registry.cpp util.cpp)
add_library(cellz SHARED CellA.cpp CellB.cpp)
#add_library(cellB SHARED cellb.cpp)
add_executable(runner loader.cpp)
target_link_libraries(registry dl boost_thread)
target_link_libraries(runner registry)

set_target_properties(runner registry cellz
    PROPERTIES
    COMPILE_FLAGS "-Wall -Werror"
    )
