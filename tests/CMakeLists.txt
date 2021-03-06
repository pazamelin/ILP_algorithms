set(CMAKE_MODULE_PATH               "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS                 "${CMAKE_CXX_FLAGS} -std=c++17 -DIL_STD -m64 -Wall -Werror")
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEBUG} -Og -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O3 -flto")
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX   "-isystem ")

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/cmake-build-debug")
elseif(CMAKE_BUILD_TYPE STREQUAL Release)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/cmake-build-release")
endif()

set(LINKER_OPTIONS                  -flto -Wl,--no-as-needed)

if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    set(CMAKE_CXX_FLAGS_DEBUG       "${CMAKE_CXX_FLAGS_DEBUG} -save-temps=obj")
endif()

macro(register_test TEST_NAME)
    if(ILPLIB_BUILD_TESTS)
        # Tests need to be added as executables first
        add_executable(${TEST_NAME} ${TEST_NAME}.cpp)

        # I'm using C++17 in the test
        target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)

        # Should be linked to the main library, as well as the Catch2 testing library
        target_link_libraries(${TEST_NAME} libilp Catch2)

        # If you register a test, then ctest and make test will run it.
        # You can also run examples and check the output, as well.
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME}) # Command can be a target
    endif()
endmacro(register_test)

register_test(eisenbrand_weismantel)
register_test(jansen_rohwedder)
register_test(random)

find_package(Cplex)
include_directories(SYSTEM ${CPLEX_INCLUDE_DIRS})

macro(register_test_CPLEX TEST_NAME)
    if(NOT CPLEX_ROOT_DIR STREQUAL "")
        # Tests need to be added as executables first
        add_executable(${TEST_NAME} ${TEST_NAME}.cpp)

        # I'm using C++17 in the test
        target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)

        # Should be linked to the main library, as well as the Catch2 testing library
        target_link_libraries(${TEST_NAME} PRIVATE libilp)

        target_include_directories(${TEST_NAME} PRIVATE ${CPLEX_INCLUDE_DIRS})

        # Link CPLEX
        target_link_libraries(${TEST_NAME} PRIVATE ${CPLEX_LIBRARIES})
        target_link_libraries(${TEST_NAME} PRIVATE dl)

        # If you register a test, then ctest and make test will run it.
        # You can also run examples and check the output, as well.
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME}) # Command can be a target
    endif()
endmacro(register_test_CPLEX)

register_test_CPLEX(cplex)

