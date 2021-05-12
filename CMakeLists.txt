# Works with 3.11 and tested through 3.20
cmake_minimum_required(VERSION 3.11...3.20)

# Project name and a few useful settings. Other commands can pick up the results
project(
        ILP_algorithms
        VERSION 0.1
        DESCRIPTION ""
        LANGUAGES CXX)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here

    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app
    # Note this needs to be done in the main CMakeLists
    # since it calls enable_testing, which must be in the
    # main CMakeLists.
    include(CTest)
endif()

# FetchContent added in CMake 3.11, downloads during the configure step
include(FetchContent)
# FetchContent_MakeAvailable was not added until CMake 3.14; use our shim
if(${CMAKE_VERSION} VERSION_LESS 3.14)
    include(cmake/add_FetchContent_MakeAvailable.cmake)
endif()

# Header-only Boost
find_package(Boost REQUIRED)

# The compiled library code is here
add_subdirectory(src)

# Testing option
option(ILPLIB_BUILD_TESTS "build tests executables" ON)
message(STATUS "ILPLIB_BUILD_TESTS : ${ILPLIB_BUILD_TESTS}")

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR ILPLIB_BUILD_TESTS)
    add_subdirectory(tests)
endif()
