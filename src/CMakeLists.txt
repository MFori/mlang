cmake_minimum_required(VERSION 3.17)
cmake_policy(SET CMP0091 NEW)
project(mlang)

macro(replace_flags flag newflag)
    FOREACH (flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
        IF (${flag_var} MATCHES ${flag})
            STRING(REGEX REPLACE "${flag}" "${newflag}" ${flag_var} "${${flag_var}}")
        ENDIF ()
    ENDFOREACH (flag_var)
endmacro()


set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
SET(CMAKE_BUILD_TYPE Debug CACHE STRING "default to debug" FORCE)
set(CMAKE_MODULE_PATH "${mlang_SOURCE_DIR}/cmake")

find_package(FLEX)
find_package(BISON)

set(LLVM_ROOT "C:/Program Files/LLVM/11.0.0/llvm/build")
set(LLVM_DIR "C:/Program Files/LLVM/11.0.0/llvm/build")
set(LLVM_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_C_FLAGS_RELEASE "/MT")
find_package(LLVM PATHS "${LLVM_ROOT}/lib/cmake/llvm" "$ENV{ProgramFiles}/lib/cmake/llvm")

if (LLVM_FOUND)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_DIR}")
else ()
    message(FATAL_ERROR "LLVM installation no found.")
endif ()

include(LLVMConfig)
message(STATUS "Found LLVM version: ${LLVM_PACKAGE_VERSION}")

llvm_map_components_to_libnames(REQ_LLVM_LIBRARIES mcjit interpreter native ipo)

set(SOURCES_COMMON
        main.cpp
        codegen.cpp
        ast.cpp
        variable.cpp
        binaryop.cpp
        return.cpp
        assignment.cpp
        buildins.cpp
        function.cpp
        unaryop.cpp
        conditional.cpp
        comparison.cpp
        whileloop.cpp
        forloop.cpp
        range.cpp
        break.cpp
        ternaryop.cpp
        array.cpp
        free.cpp
        str.cpp
        foreach.cpp
        cast.cpp
        lexer.l
        parser.y
        )

set(HEADER_COMMON
        codegen.h
        ast.h
        visitor.h
        variable.h
        binaryop.h
        return.h
        assignment.h
        buildins.h
        function.h
        unaryop.h
        conditional.h
        comparison.h
        whileloop.h
        forloop.h
        ternaryop.h
        range.h
        break.h
        array.h
        free.h
        str.h
        foreach.h
        cast.h
        )

if (MSVC)
    source_group(Header\ Files FILES ${HEADER_COMMON})
    source_group(Source\ Files FILES ${SOURCE_COMMON})
endif ()

set(VER_MAJ 0)
set(VER_MIN 3)
set(VER_REV 1)


FLEX_TARGET(Lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
BISON_TARGET(Parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp COMPILE_FLAGS --report=solved COMPILE_FLAGS --feature=fixit)
ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

add_executable(mlang ${SOURCES_COMMON} ${HEADER_COMMON} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(mlang PRIVATE -Wall)
elseif (MSVC)
    replace_flags("/MDd" "/MTd")
    replace_flags("/MD" "/MT")
    target_compile_options(mlang PRIVATE /W4 /permissive-)
    target_compile_definitions(mlang PRIVATE YY_NO_UNISTD_H)
    target_compile_definitions(mlang PRIVATE _SCL_SECURE_NO_WARNINGS)
    target_compile_definitions(mlang PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(mlang PRIVATE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/tokens.cpp PROPERTIES COMPILE_DEFINITIONS __STDC_VERSION__=199901L)
    string(REPLACE "-D" ";" LLVM_DEFINITIONS ${LLVM_DEFINITIONS})
endif ()

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

target_compile_definitions(mlang PRIVATE $<$<CONFIG:Debug>:_DEBUG>)
target_compile_definitions(mlang PRIVATE ${LLVM_DEFINITIONS})
target_compile_definitions(mlang PRIVATE LLVM_NO_DUMP)

if (NOT LLVM_ENABLE_RTTI)
    target_compile_definitions(mlang PRIVATE mlang_NO_RTTI)
    if (MSVC)
        target_compile_options(mlang PRIVATE /GR-)
    else ()
        target_compile_options(mlang PRIVATE -fno-rtti)
    endif ()
endif ()

if (LLVM_ENABLE_EH AND CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(mlang PRIVATE -fexceptions)
endif ()

target_include_directories(mlang PRIVATE ${mlang_SOURCE_DIR} ${LLVM_INCLUDE_DIRS})
target_include_directories(mlang PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(mlang PRIVATE cxx_std_17)
target_link_libraries(mlang ${REQ_LLVM_LIBRARIES})


set_property(TARGET mlang PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")