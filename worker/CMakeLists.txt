cmake_minimum_required(VERSION 3.25)

project(app LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


Include(FetchContent)
enable_testing()

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)


add_executable(tests ./src/tests/test.cpp ./src/core/encoding.cpp)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)

add_executable(app ./src/main.cpp ./src/core/encoding.cpp)
