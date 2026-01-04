cmake_minimum_required(VERSION 3.22)

project(app LANGUAGES CXX)

# ----------------------------
# Global settings
# ----------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------
# Dependencies
# ----------------------------
include(FetchContent)

# ---- Catch2 (tests only) ----
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)
FetchContent_MakeAvailable(Catch2)

# ---- Protobuf ----
find_package(Protobuf REQUIRED)

# ---- ZooKeeper (C client) ----
# Ubuntu package: libzookeeper-mt-dev
find_library(ZOOKEEPER_LIB zookeeper_mt REQUIRED)

# ----------------------------
# Core sources (shared)
# ----------------------------
set(CORE_SOURCES
    ./src/core/huffman_tree.cpp
    ./src/core/encoding.cpp
    ./src/core/decoding.cpp
    ./src/core/bit_packer.cpp
    ./src/core/file_handler.cpp
    ./src/core/stats.cpp
)

# ----------------------------
# Protobuf generated sources
# ----------------------------
set(PROTO_SOURCES
    ./generated/huffman.pb.cc
)

# ----------------------------
# Main application
# ----------------------------
add_executable(app
    ./src/main.cpp
    ./src/core/zk_client.cpp
    ${CORE_SOURCES}
    ${PROTO_SOURCES}
)

target_link_libraries(app
    ${ZOOKEEPER_LIB}
    protobuf::libprotobuf
)

target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)


# ----------------------------
# Tests
# ----------------------------
enable_testing()

add_executable(unit_tests
    ./src/tests/test.cpp
    ${CORE_SOURCES}
    ${PROTO_SOURCES}
)

target_link_libraries(unit_tests
    Catch2::Catch2WithMain
    protobuf::libprotobuf
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)



# ---- Catch2 test discovery ----
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(unit_tests)


# cmake_minimum_required(VERSION 3.25)
# project(app LANGUAGES CXX)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_STANDARD 17) # Recommended for your map/struct usage

# # --- Fetch Catch2 ---
# include(FetchContent)
# FetchContent_Declare(
#   Catch2
#   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#   GIT_TAG        v3.8.1
# )
# FetchContent_MakeAvailable(Catch2)

# # --- Define Source Groups ---
# # These files are used by BOTH the app and the tests
# set(CORE_SOURCES 
#     ./src/core/huffman_tree.cpp 
#     ./src/core/encoding.cpp 
#     ./src/core/decoding.cpp
#     ./src/core/bit_packer.cpp
#     ./src/core/file_handler.cpp
#     ./src/core/stats.cpp

# )

# # --- Target 1: The Actual Program ---
# # Use this for your manual experiments (cout, etc.)
# add_executable(app ./src/main.cpp ${CORE_SOURCES})

# # --- Target 2: The Test Suite ---
# # Create a new file called ./src/tests.cpp for your TEST_CASEs
# # Do NOT include main.cpp here
# enable_testing()
# add_executable(unit_tests ./src/tests/test.cpp ${CORE_SOURCES})

# # Link Catch2 with its internal main() to the test target
# target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)

# # --- Catch2 Discovery ---
# list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
# include(CTest)
# include(Catch)
# # Only run discovery on the target that actually uses Catch2
# catch_discover_tests(unit_tests)
