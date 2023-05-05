project(Sudoku LANGUAGES CXX)
cmake_minimum_required(VERSION 3.16)

add_executable(${PROJECT_NAME} main.cpp SudokuPuzzle.cpp Grid.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_auto_type)
configure_file(sudoku_puzzle.txt ${CMAKE_CURRENT_BINARY_DIR}/sudoku_puzzle.txt)
#configure_file(Find.cl ${CMAKE_CURRENT_BINARY_DIR}/Find.cl)

# Requires OpenCL
#find_package(OpenCL REQUIRED)
#target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL)