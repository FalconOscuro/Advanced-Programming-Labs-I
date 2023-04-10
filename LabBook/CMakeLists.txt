# Lab book build
project(LabBook-I)
cmake_minimum_required(VERSION 3.16)

message(STATUS "Preparing for LaTeX")
find_package(LATEX)
set(MAIN_TEX "Lab-I.tex")

if(LATEX_FOUND AND LATEX_PDFLATEX_FOUND)
    # First Pass
    add_custom_target( latex-prebuild
                        COMMAND ${PDFLATEX_COMPILER} -output-directory ${CMAKE_CURRENT_BINARY_DIR} -draftmode -shell-escape -interaction=nonstopmode ${MAIN_TEX}
                        COMMENT "Starting Prebuild..."
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        DEPENDS ${MAIN_TEX}
                )
    # Second Pass, generating final pdf
    add_custom_target( latex-pdf
                        COMMAND ${PDFLATEX_COMPILER} -output-directory ${CMAKE_CURRENT_BINARY_DIR} -shell-escape ${MAIN_TEX}
                        COMMENT "Starting Final Build..."
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        DEPENDS ${MAIN_TEX}
                )
    add_dependencies(latex-pdf latex-prebuild)
    add_custom_target(all-formats ALL) # Entry point for execution
    add_dependencies(all-formats latex-pdf)
else()
    message("LaTeX tools not found, unable to build tex files")
endif()