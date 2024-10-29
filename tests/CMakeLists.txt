set(CMAKE_CXX_COMPILER "/opt/homebrew/Cellar/llvm/19.1.2/bin/clang++")
set(CMAKE_LINKER "/opt/homebrew/Cellar/llvm/19.1.2/bin/ld.lld")

add_executable(yumina_tests
        arch/test.cpp
)

target_link_libraries(yumina_tests PRIVATE
        Yumina
        GTest::gtest
        GTest::gtest_main
)

# Architecture-specific configurations
if(YUMINA_ARCH_X64)
    target_compile_definitions(yumina_tests PRIVATE YUMINA_ARCH_X64)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(yumina_tests PRIVATE
                -mavx2
                -mavx512f
                -msse4.2
        )
    elseif(MSVC)
        target_compile_options(yumina_tests PRIVATE /arch:AVX2)
    endif()
elseif(YUMINA_ARCH_ARM64)
    target_compile_definitions(yumina_tests PRIVATE YUMINA_ARCH_ARM64)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(yumina_tests PRIVATE
                -march=armv8-a+crypto+crc
        )

        # macOS ARM64 specific configurations for LLVM
        if(APPLE)
            # Add LLVM lib directory
            target_link_directories(yumina_tests PRIVATE
                    /opt/homebrew/Cellar/llvm/19.1.2/lib
            )

            # Explicitly specify C++ standard library
            target_compile_options(yumina_tests PRIVATE
                    -stdlib=libc++
            )

            # Link against libc++ and libc++abi
            target_link_options(yumina_tests PRIVATE
                    -stdlib=libc++
                    -lc++
                    -lc++abi
            )

            # Ensure LLVM's lib path is searched
            set_target_properties(yumina_tests PROPERTIES
                    LINK_FLAGS "-L/opt/homebrew/Cellar/llvm/19.1.2/lib"
            )
        endif()
    endif()
endif()

# Compiler warning flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(yumina_tests PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wno-deprecated-declarations  # Suppress Google Test warnings
    )
elseif(MSVC)
    target_compile_options(yumina_tests PRIVATE /W4)
endif()

# Register test
include(GoogleTest)
gtest_discover_tests(yumina_tests)