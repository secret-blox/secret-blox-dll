# This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
if(EXT_PLATFORM_STRING)
    include(EXTLuau.cmake)
    return()
endif()

cmake_minimum_required(VERSION 3.0)

option(LUAU_BUILD_CLI "Build CLI" OFF)
option(LUAU_BUILD_TESTS "Build tests" OFF)
option(LUAU_BUILD_WEB "Build Web module" OFF)
option(LUAU_WERROR "Warnings as errors" OFF)
option(LUAU_STATIC_CRT "Link with the static CRT (/MT)" OFF)
option(LUAU_EXTERN_C "Use extern C for all APIs" ON)

cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0091 NEW)

if(LUAU_STATIC_CRT)
    cmake_minimum_required(VERSION 3.15)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(Luau LANGUAGES CXX C)
add_library(Luau.Common INTERFACE)
add_library(Luau.Ast STATIC)
add_library(Luau.Compiler STATIC)
add_library(Luau.VM STATIC)

# Proxy target to make it possible to depend on private VM headers
add_library(Luau.VM.Internals INTERFACE)

include(Sources.cmake)

target_include_directories(Luau.Common INTERFACE Common/include)

target_compile_features(Luau.Ast PUBLIC cxx_std_17)
target_include_directories(Luau.Ast PUBLIC Ast/include)
target_link_libraries(Luau.Ast PUBLIC Luau.Common)

target_compile_features(Luau.Compiler PUBLIC cxx_std_17)
target_include_directories(Luau.Compiler PUBLIC Compiler/include)
target_link_libraries(Luau.Compiler PUBLIC Luau.Ast)

target_compile_features(Luau.VM PRIVATE cxx_std_11)
target_include_directories(Luau.VM PUBLIC VM/include)
target_link_libraries(Luau.VM PUBLIC Luau.Common)

target_include_directories(Luau.VM.Internals INTERFACE VM/src)

set(LUAU_OPTIONS)

if(MSVC)
    list(APPEND LUAU_OPTIONS /D_CRT_SECURE_NO_WARNINGS) # We need to use the portable CRT functions.
    list(APPEND LUAU_OPTIONS "/we4018") # Signed/unsigned mismatch
    list(APPEND LUAU_OPTIONS "/we4388") # Also signed/unsigned mismatch
else()
    list(APPEND LUAU_OPTIONS -Wall) # All warnings
    list(APPEND LUAU_OPTIONS -Wsign-compare) # This looks to be included in -Wall for GCC but not clang
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND LUAU_OPTIONS /MP) # Distribute single project compilation across multiple cores
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Some gcc versions treat var in `if (type var = val)` as unused
    # Some gcc versions treat variables used in constexpr if blocks as unused
    list(APPEND LUAU_OPTIONS -Wno-unused)
endif()

# Enabled in CI; we should be warning free on our main compiler versions but don't guarantee being warning free everywhere
if(LUAU_WERROR)
    if(MSVC)
        list(APPEND LUAU_OPTIONS /WX) # Warnings are errors
    else()
        list(APPEND LUAU_OPTIONS -Werror) # Warnings are errors
    endif()
endif()

if(LUAU_BUILD_WEB)
    # add -fexceptions for emscripten to allow exceptions to be caught in C++
    list(APPEND LUAU_OPTIONS -fexceptions)
endif()

set(ISOCLINE_OPTIONS)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND ISOCLINE_OPTIONS -Wno-unused-function)
endif()

target_compile_options(Luau.Ast PRIVATE ${LUAU_OPTIONS})
target_compile_options(Luau.VM PRIVATE ${LUAU_OPTIONS})

if(LUAU_EXTERN_C)
    # enable extern "C" for VM (lua.h, lualib.h) and Compiler (luacode.h) to make Luau friendlier to use from non-C++ languages
    # note that we enable LUA_USE_LONGJMP=1 as well; otherwise functions like luaL_error will throw C++ exceptions, which can't be done from extern "C" functions
    target_compile_definitions(Luau.VM PUBLIC LUA_USE_LONGJMP=1)
    target_compile_definitions(Luau.VM PUBLIC LUA_API=extern\"C\")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND MSVC_VERSION GREATER_EQUAL 1924)
    # disable partial redundancy elimination which regresses interpreter codegen substantially in VS2022:
    # https://developercommunity.visualstudio.com/t/performance-regression-on-a-complex-interpreter-lo/1631863
    set_source_files_properties(VM/src/lvmexecute.cpp PROPERTIES COMPILE_FLAGS /d2ssa-pre-)
endif()

if (NOT MSVC)
    # disable support for math_errno which allows compilers to lower sqrt() into a single CPU instruction
    target_compile_options(Luau.VM PRIVATE -fno-math-errno)
endif()

# validate dependencies for internal libraries
foreach(LIB Luau.Ast Luau.Compiler Luau.Config Luau.Analysis Luau.CodeGen Luau.VM)
    if(TARGET ${LIB})
        get_target_property(DEPENDS ${LIB} LINK_LIBRARIES)
        if(LIB MATCHES "CodeGen|VM" AND DEPENDS MATCHES "Ast|Analysis|Config|Compiler")
            message(FATAL_ERROR ${LIB} " is a runtime component but it depends on one of the offline components")
        endif()
        if(LIB MATCHES "Ast|Analysis|Compiler" AND DEPENDS MATCHES "CodeGen|VM")
            message(FATAL_ERROR ${LIB} " is an offline component but it depends on one of the runtime components")
        endif()
        if(LIB MATCHES "Ast|Compiler" AND DEPENDS MATCHES "Analysis|Config")
            message(FATAL_ERROR ${LIB} " is a compiler component but it depends on one of the analysis components")
        endif()
    endif()
endforeach()
