cmake_minimum_required(VERSION 3.25)
project(app LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17) # Recommended for your map/struct usage

# --- Fetch Catch2 ---
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)
FetchContent_MakeAvailable(Catch2)

# --- Define Source Groups ---
# These files are used by BOTH the app and the tests
set(CORE_SOURCES 
    ./src/core/huffman_tree.cpp 
    ./src/core/encoding.cpp 
    ./src/core/decoding.cpp
    ./src/core/bit_packer.cpp
    ./src/core/file_handler.cpp
    ./src/core/stats.cpp

)

# --- Target 1: The Actual Program ---
# Use this for your manual experiments (cout, etc.)
add_executable(app ./src/main.cpp ${CORE_SOURCES})

# --- Target 2: The Test Suite ---
# Create a new file called ./src/tests.cpp for your TEST_CASEs
# Do NOT include main.cpp here
enable_testing()
add_executable(unit_tests ./src/tests/test.cpp ${CORE_SOURCES})

# Link Catch2 with its internal main() to the test target
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)

# --- Catch2 Discovery ---
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
# Only run discovery on the target that actually uses Catch2
catch_discover_tests(unit_tests)





# cmake_minimum_required(VERSION 3.25)

# project(app LANGUAGES CXX)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Include(FetchContent)
# enable_testing()

# FetchContent_Declare(
#   Catch2
#   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#   GIT_TAG        v3.8.1 # or a later release
# )

# FetchContent_MakeAvailable(Catch2)


# add_executable(tests ./src/main.cpp ./src/core/huffman_tree.cpp ./src/core/encoding.cpp ./src/core/decoding.cpp )


# target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
# include(CTest)
# include(Catch)
# catch_discover_tests(tests)

# add_executable(app ./src/main.cpp ./src/core/huffman_tree.cpp ./src/core/encoding.cpp ./src/core/decoding.cpp )
