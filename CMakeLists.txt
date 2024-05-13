cmake_minimum_required(VERSION 3.29)
project(DS)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(icb_lib)
#add_executable(DS src/main.cpp)
#target_link_libraries(DS PRIVATE icb_lib -fsanitize=address)
#target_compile_options(DS PRIVATE -Werror -pedantic-errors -Wall -Wextra -Wconversion -Wsign-conversion -fsized-deallocation -fsanitize=address)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES "tests/*.cc" RECURSIVE TESTS)

add_executable(all_tests ${TEST_SOURCES})

target_link_libraries(all_tests PRIVATE icb_lib GTest::gtest_main)

enable_testing()

include(GoogleTest)
gtest_discover_tests(all_tests)