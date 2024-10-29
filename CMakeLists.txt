cmake_minimum_required(VERSION 3.29)
project(Yumina)

set(CMAKE_CXX_STANDARD 20)

add_library(Yumina STATIC
        memory/memory.cpp
        memory/memory.h
        special/arch.hpp
)

option(YUMINA_BUILD_TESTS "Build Yumina tests" ON)
if (YUMINA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(vendor/googletest)
    add_subdirectory(tests)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Yumina PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            $<$<BOOL:${YUMINA_ARCH_X64}>:-mavx2 -mavx512f -msse4.2>
            $<$<BOOL:${YUMINA_ARCH_ARM64}>:-march=armv8-a+crypto+crc>
    )
elseif(MSVC)
    target_compile_options(Yumina PRIVATE
    /W4
    /arch:AVX2
    )
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "amd64|x86_64")
    set(YUMINA_ARCH_X64 ON)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(YUMINA_ARCH_ARM64 ON)
endif()