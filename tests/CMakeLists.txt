set(CMAKE_BUILD_TYPE Release)

if(APPLE AND YUMINA_ARCH_ARM64)
    # Already set in main CMakeLists.txt, but reinforce here
    add_compile_options(-stdlib=libc++)
    add_link_options(-stdlib=libc++)

    find_library(LIBCXX_LIBRARY c++)
    find_library(LIBCXXABI_LIBRARY c++abi)

    if(NOT LIBCXX_LIBRARY OR NOT LIBCXXABI_LIBRARY)
        message(FATAL_ERROR "Could not find libc++ or libc++abi")
    endif()
endif()

add_executable(yumina_tests
        arch/test.cpp
)

target_include_directories(yumina_tests PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest/include
)

# Architecture-specific configurations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(yumina_tests PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wno-deprecated-declarations
    )

    if(YUMINA_ARCH_X64)
        target_compile_options(yumina_tests PRIVATE -mavx2 -mavx512f -msse4.2)
        target_compile_definitions(yumina_tests PRIVATE YUMINA_ARCH_X64)
    elseif(YUMINA_ARCH_ARM64)
        target_compile_options(yumina_tests PRIVATE -march=armv8-a+crypto+crc)
        target_compile_definitions(yumina_tests PRIVATE YUMINA_ARCH_ARM64)

        if(APPLE)
            target_link_options(yumina_tests PRIVATE
                    "LINKER:-rpath,${CMAKE_BINARY_DIR}/lib"
            )

            target_link_libraries(yumina_tests PRIVATE
                    ${LIBCXX_LIBRARY}
                    ${LIBCXXABI_LIBRARY}
                    "-framework CoreFoundation"
            )
        endif()
    endif()
elseif(MSVC)
    target_compile_options(yumina_tests PRIVATE /W4)
    if(YUMINA_ARCH_X64)
        target_compile_options(yumina_tests PRIVATE /arch:AVX2)
    endif()
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
    add_definitions(-DYUMINA_ARCH_X64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    add_definitions(-DYUMINA_ARCH_ARM64)
endif()

target_link_libraries(yumina_tests PRIVATE
        Yumina
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(yumina_tests
        PROPERTIES
        ENVIRONMENT "GTEST_COLOR=yes"
)