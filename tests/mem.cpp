#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include "../memory/allocator.h"

TEST(allocator, alloc_dealloc)
{
    const auto ptr = static_cast<int *>(yumina::detail::internal::allocate(100));
    ASSERT_NE(ptr, nullptr);
    yumina::detail::internal::deallocate(ptr);
}

TEST(allocator, alloc_dealloc_array)
{
    const auto ptr = static_cast<int *>(yumina::detail::internal::allocate(100 * sizeof(int)));
    ASSERT_NE(ptr, nullptr);
    yumina::detail::internal::deallocate(ptr);
}

TEST(AllocatorTest, NewDelete)
{
    const auto ptr = new int(42);
    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(*ptr, 42);
    delete ptr;
}

TEST(allocator, alloc_zero_size)
{
    const auto ptr = static_cast<int *>(yumina::detail::internal::allocate(0));
    ASSERT_EQ(ptr, nullptr);
}

TEST(allocator, alloc_large_size)
{
    const auto ptr = static_cast<int *>(yumina::detail::internal::allocate(1024 * 1024 * 1024)); // 1 GB
    ASSERT_NE(ptr, nullptr);
    yumina::detail::internal::deallocate(ptr);
}

TEST(allocator, reallocate_zero)
{
    auto ptr = static_cast<int*>(yumina::detail::internal::allocate(sizeof(int)));
    ASSERT_NE(ptr, nullptr);

    auto new_ptr = static_cast<int*>(yumina::detail::internal::reallocate(ptr, 0));
    ASSERT_EQ(new_ptr, nullptr);
}

TEST(allocator, calloc_basic)
{
    size_t num_elements = 5;
    size_t element_size = sizeof(int);
    auto ptr = static_cast<int*>(yumina::detail::internal::callocate(num_elements, element_size));
    ASSERT_NE(ptr, nullptr);

    for(size_t i = 0; i < num_elements; ++i)
        ASSERT_EQ(ptr[i], 0);

    yumina::detail::internal::deallocate(ptr);
}

TEST(allocator, calloc_zero_count)
{
    const auto ptr = static_cast<int*>(yumina::detail::internal::callocate(0, sizeof(int)));
    ASSERT_EQ(ptr, nullptr);
}

TEST(allocator, calloc_zero_size)
{
    const auto ptr = static_cast<int*>(yumina::detail::internal::callocate(5, 0));
    ASSERT_EQ(ptr, nullptr);
}

TEST(allocator, alignment_check)
{
    for(const size_t size : {1, 2, 4, 8, 16, 32})
    {
        auto ptr = yumina::detail::internal::allocate(size);
        ASSERT_NE(ptr, nullptr);

        ASSERT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % std::alignment_of<std::max_align_t>::value, 0);
        yumina::detail::internal::deallocate(ptr);
    }
}

TEST(allocator, multiple_allocations)
{
    std::vector<void*> ptrs;
    constexpr int num_allocs = 10;

    for(auto i = 0; i < num_allocs; i++)
    {
        auto ptr = yumina::detail::internal::allocate(i + 1);
        ASSERT_NE(ptr, nullptr);
        ptrs.push_back(ptr);
    }

    while(!ptrs.empty())
    {
        yumina::detail::internal::deallocate(ptrs.back());
        ptrs.pop_back();
    }
}

TEST(allocator, reallocate_null_ptr)
{
    auto ptr = static_cast<int*>(yumina::detail::internal::reallocate(nullptr, sizeof(int) * 5));
    ASSERT_NE(ptr, nullptr);
    yumina::detail::internal::deallocate(ptr);
}

TEST(allocator, boundary_values)
{
    ASSERT_EQ(yumina::detail::internal::allocate(std::numeric_limits<size_t>::max()), nullptr);
    ASSERT_EQ(yumina::detail::internal::allocate(std::numeric_limits<size_t>::max() - 1), nullptr);
}

TEST(allocator, mixed_sizes)
{
    void* ptr1 = yumina::detail::internal::allocate(100);
    void* ptr2 = yumina::detail::internal::allocate(50);
    void* ptr3 = yumina::detail::internal::allocate(200);

    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr3, nullptr);

    yumina::detail::internal::deallocate(ptr2);
    yumina::detail::internal::deallocate(ptr1);
    yumina::detail::internal::deallocate(ptr3);
}

TEST(allocator, realloc_preserve_content)
{
    auto ptr = static_cast<unsigned char*>(yumina::detail::internal::allocate(8));
    ASSERT_NE(ptr, nullptr);

    for(size_t i = 0; i < 5; i++)
        ptr[i] = static_cast<uint8_t>(i);

    ptr = static_cast<unsigned char*>(yumina::detail::internal::reallocate(ptr, 64));
    if (!ptr)
        return;

    for(size_t i = 0; i < 5; i++)
        ASSERT_EQ(ptr[i], static_cast<unsigned char>(i));

    yumina::detail::internal::deallocate(ptr);
}

TEST(allocator, calloc_large_arrays)
{
    constexpr size_t num_elements = 1024 * 1024;
    auto ptr = static_cast<char*>(yumina::detail::internal::callocate(num_elements, sizeof(char)));
    ASSERT_NE(ptr, nullptr);

    for(size_t i = 0; i < num_elements; ++i)
        ASSERT_EQ(ptr[i], 0);

    yumina::detail::internal::deallocate(ptr);
}

TEST(allocator, tiny_allocation)
{
    for (size_t size = 8; size <= 64; size += 8)
    {
        auto ptr = yumina::detail::internal::allocate(size);
        ASSERT_NE(ptr, nullptr);

        std::memset(ptr, 0xAA, size);

        yumina::detail::internal::deallocate(ptr);
    }
}

TEST(allocator, small_allocation)
{
    for (size_t size = 128; size <= 1024; size *= 2)
    {
        auto ptr = yumina::detail::internal::allocate(size);
        ASSERT_NE(ptr, nullptr);

        auto* bytes = static_cast<unsigned char*>(ptr);
        for (size_t i = 0; i < size; ++i)
            bytes[i] = static_cast<unsigned char>(i & 0xFF);


        yumina::detail::internal::deallocate(ptr);
    }
}

TEST(allocator, realloc_same_class)
{
    constexpr size_t initial_size = 32;
    auto ptr = static_cast<char*>(yumina::detail::internal::allocate(initial_size));
    ASSERT_NE(ptr, nullptr);

    for (size_t i = 0; i < initial_size; ++i)
        ptr[i] = static_cast<char>(i);

    constexpr size_t new_size = 48;
    const auto new_ptr = static_cast<char*>(yumina::detail::internal::reallocate(ptr, new_size));
    if (!new_ptr)
        return;

    for (size_t i = 0; i < initial_size; ++i)
        EXPECT_EQ(new_ptr[i], static_cast<char>(i));

    yumina::detail::internal::deallocate(new_ptr);
}

TEST(allocator, realloc_cross_class)
{
    constexpr size_t initial_size = 32;
    auto ptr = static_cast<char*>(yumina::detail::internal::allocate(initial_size));
    ASSERT_NE(ptr, nullptr);

    for (size_t i = 0; i < initial_size; ++i)
        ptr[i] = static_cast<char>(i);

    constexpr size_t new_size = 128;
    const auto new_ptr = static_cast<char*>(yumina::detail::internal::reallocate(ptr, new_size));
    if (!new_ptr)
        return;

    for (size_t i = 0; i < initial_size; ++i)
        EXPECT_EQ(new_ptr[i], static_cast<char>(i));

    yumina::detail::internal::deallocate(new_ptr);
}

TEST(allocator, aligned_allocation)
{
    // Test cache line alignment
    constexpr size_t cache_line_size = 64;
    auto ptr = yumina::detail::internal::allocate(cache_line_size);
    if (!ptr)
        return;
    auto alignment = reinterpret_cast<uintptr_t>(ptr) % cache_line_size;
    EXPECT_EQ(alignment, 0)
        << "Expected 64-byte alignment, but pointer "
        << ptr << " is offset by " << alignment << " bytes";
    yumina::detail::internal::deallocate(ptr);

    constexpr size_t page_size = 4096;
    ptr = yumina::detail::internal::allocate(page_size);
    if (!ptr)
        return;
    alignment = reinterpret_cast<uintptr_t>(ptr) % page_size;
    EXPECT_EQ(alignment, 64)
        << "Expected page alignment, but pointer "
        << ptr << " is offset by " << alignment << " bytes";
    yumina::detail::internal::deallocate(ptr);
}

TEST(allocator, calloc_sizes)
{
    {
        auto ptr = static_cast<char*>(yumina::detail::internal::callocate(8, 1));
        ASSERT_NE(ptr, nullptr);
        for (size_t i = 0; i < 8; ++i)
            EXPECT_EQ(ptr[i], 0);
        yumina::detail::internal::deallocate(ptr);
    }

    {
        auto ptr = static_cast<int*>(yumina::detail::internal::callocate(256, sizeof(int)));
        ASSERT_NE(ptr, nullptr);
        for (size_t i = 0; i < 256; ++i)
            EXPECT_EQ(ptr[i], 0);
        yumina::detail::internal::deallocate(ptr);
    }
}

TEST(allocator, realloc_edge_cases)
{
    {
        const auto ptr = yumina::detail::internal::reallocate(nullptr, 64);
        ASSERT_NE(ptr, nullptr);
        yumina::detail::internal::deallocate(ptr);
    }

    {
        const auto ptr = yumina::detail::internal::allocate(64);
        ASSERT_NE(ptr, nullptr);
        auto new_ptr = yumina::detail::internal::reallocate(ptr, 0);
        EXPECT_EQ(new_ptr, nullptr);
    }

    {
        const auto ptr = yumina::detail::internal::allocate(64);
        if (!ptr)
            return;
        const auto new_ptr = yumina::detail::internal::reallocate(ptr, 64);
        if (!new_ptr)
            return;
        yumina::detail::internal::deallocate(new_ptr);
    }
}

TEST(allocator, multiple_alloc_dealloc)
{
    std::vector<void*> ptrs;
    constexpr auto num_allocs = 10;

    for (auto i = 0; i < num_allocs; ++i)
    {
        auto ptr = yumina::detail::internal::allocate((i + 1) * 8);
        if (!ptr)
            continue;
        ptrs.push_back(ptr);
    }

    while (!ptrs.empty())
    {
        yumina::detail::internal::deallocate(ptrs.back());
        ptrs.pop_back();
    }
}