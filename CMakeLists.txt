project(Lab-G)
cmake_minimum_required(VERSION 3.16)

# Task files
# add_subdirectory(Tasks)

# Only build lab book if this is the top level directory
if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    option(BUILD_LABBOOK "Build the labbook" OFF)

    if (BUILD_LABBOOK)
        add_subdirectory(LabBook)
    endif()
endif()