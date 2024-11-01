set(CMAKE_BUILD_TYPE Release)

# Platform-specific settings for Apple ARM64
if(APPLE AND YUMINA_ARCH_ARM64)
    add_compile_options(-stdlib=libc++)
    add_link_options(-stdlib=libc++)
    find_library(LIBCXX_LIBRARY c++)
    find_library(LIBCXXABI_LIBRARY c++abi)
    if(NOT LIBCXX_LIBRARY OR NOT LIBCXXABI_LIBRARY)
        message(FATAL_ERROR "Could not find libc++ or libc++abi")
    endif()
endif()

# Create test executable
add_executable(yumina_tests mem.cpp)

# Core include directories
target_include_directories(yumina_tests PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/memory
        ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest/include
)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(yumina_tests PRIVATE
            -Wall -Wextra -Wpedantic -Wno-deprecated-declarations
            $<$<BOOL:${YUMINA_ARCH_X64}>:-mavx2 -mavx512f -msse4.2>
            $<$<BOOL:${YUMINA_ARCH_ARM64}>:-march=armv8-a+crypto+crc>
    )
elseif(MSVC)
    target_compile_options(yumina_tests PRIVATE
            /W4
            $<$<BOOL:${YUMINA_ARCH_X64}>:/arch:AVX2>
    )
endif()

target_compile_definitions(yumina_tests PRIVATE
        $<$<OR:$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},x86_64>,$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},AMD64>>:YUMINA_ARCH_X64>
        $<$<OR:$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},aarch64>,$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},arm64>>:YUMINA_ARCH_ARM64>
)

target_link_libraries(yumina_tests PRIVATE
        Yumina
        GTest::gtest
        GTest::gtest_main
        $<$<AND:$<BOOL:${APPLE}>,$<BOOL:${YUMINA_ARCH_ARM64}>>:${LIBCXX_LIBRARY}>
        $<$<AND:$<BOOL:${APPLE}>,$<BOOL:${YUMINA_ARCH_ARM64}>>:${LIBCXXABI_LIBRARY}>
)

# Add rpath for Apple ARM64
if(APPLE AND YUMINA_ARCH_ARM64)
    target_link_options(yumina_tests PRIVATE "LINKER:-rpath,${CMAKE_BINARY_DIR}/lib")
endif()

include(GoogleTest)
gtest_discover_tests(yumina_tests PROPERTIES ENVIRONMENT "GTEST_COLOR=yes")