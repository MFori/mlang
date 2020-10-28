cmake_minimum_required(VERSION 3.18)
project(mlang)

FIND_PACKAGE(FLEX)
FIND_PACKAGE(BISON)

set(SOURCES_COMMON
        main.cpp
        lexer.l
        parser.y)

set(HEADER_COMMON
        )

FLEX_TARGET(Lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
BISON_TARGET(Parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp COMPILE_FLAGS --report=solved COMPILE_FLAGS --feature=fixit)
ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

add_executable(mlang ${SOURCE_COMMON} ${HEADER_COMMON} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})