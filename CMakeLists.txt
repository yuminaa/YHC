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
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

add_library(Yumina STATIC
        memory/memory.h
        common/arch.hpp
        memory/allocator.cpp
        memory/allocator.h
        common/utils.hpp
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

# Add the CoreFoundation framework for macOS
if(APPLE)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    if(NOT COREFOUNDATION_FRAMEWORK)
        message(FATAL_ERROR "Could not find CoreFoundation framework")
    endif()
    target_link_libraries(Yumina PRIVATE ${COREFOUNDATION_FRAMEWORK})
endif()