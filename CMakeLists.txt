cmake_minimum_required(VERSION 3.12)
project(diogenes)

set(CMAKE_CXX_STANDARD 14)

find_package(Catch2 REQUIRED)

include_directories("include")

# build executable
add_executable(
        diogenes
        src/main.cpp
        src/attacks.cpp
        src/bitboard.cpp
        src/board.cpp
        src/evaluation.cpp
        src/generator.cpp
        src/gui.cpp
        src/main.cpp
        src/move.cpp
        src/search.cpp
        src/uci.cpp
        src/utils.cpp

)
target_include_directories(
        diogenes
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_sources(
        diogenes
        INTERFACE
        src/attacks.cpp
        src/bitboard.cpp
        src/board.cpp
        src/evaluation.cpp
        src/generator.cpp
        src/gui.cpp
        src/main.cpp
        src/move.cpp
        src/search.cpp
        src/uci.cpp
        src/utils.cpp
)

# test executable
add_executable(
        tests
        test/tests-main.cpp
        src/attacks.cpp
        src/bitboard.cpp
        src/board.cpp
        src/evaluation.cpp
        src/generator.cpp
        src/gui.cpp
        src/move.cpp
        src/search.cpp
        src/uci.cpp
        src/utils.cpp
        test/test-attacks.cpp
        test/tests-bitboards.cpp
        test/tests-board.cpp
        test/tests-main.cpp
        test/tests-move.cpp
        test/tests-movegen.cpp
        test/tests-perft.cpp
        test/tests-search.cpp
        test/tests-utils.cpp

)
target_include_directories(
        tests
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_link_libraries(tests PRIVATE Catch2::Catch2)