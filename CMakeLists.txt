cmake_minimum_required(VERSION 3.29)
project(Yumina)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "amd64|x86_64")
    set(YUMINA_ARCH_X64 ON)
    set(YUMINA_ARCH_ARM64 OFF)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(YUMINA_ARCH_X64 OFF)
    set(YUMINA_ARCH_ARM64 ON)
endif()

# macOS ARM64 specific configurations
if(APPLE AND YUMINA_ARCH_ARM64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

add_library(Yumina STATIC
        memory/memory.cpp
        memory/memory.h
        special/arch.hpp
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Yumina PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )

    if(YUMINA_ARCH_X64)
        target_compile_options(Yumina PRIVATE -mavx2 -mavx512f -msse4.2)
    elseif(YUMINA_ARCH_ARM64)
        target_compile_options(Yumina PRIVATE -march=armv8-a+crypto+crc)
    endif()
elseif(MSVC)
    target_compile_options(Yumina PRIVATE /W4 $<$<BOOL:${YUMINA_ARCH_X64}>:/arch:AVX2>)
endif()

option(YUMINA_BUILD_TESTS "Build Yumina tests" ON)
if(YUMINA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(vendor/googletest)
    add_subdirectory(tests)
endif()