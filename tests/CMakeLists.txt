# Testing library
FetchContent_Declare(
        catch
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.9.1)
FetchContent_MakeAvailable(catch)
# Adds Catch2::Catch2

# Tests need to be added as executables first
add_executable(libtest unit.cpp)
add_executable(eisenbrand_weismantel eisenbrand_weismantel.cpp)

# I'm using C++17 in the test
target_compile_features(libtest PRIVATE cxx_std_17)
target_compile_features(eisenbrand_weismantel PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(libtest PRIVATE libilp Catch2::Catch2)
target_link_libraries(eisenbrand_weismantel PRIVATE libilp Catch2::Catch2)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME test_libtest COMMAND libtest) # Command can be a target
add_test(NAME eisenbrand_weismantel COMMAND eisenbrand_weismantel) # Command can be a target
