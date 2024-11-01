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

TEST(AllocatorTest, NewDeleteArray)
{
    const auto ptr = new int[100];
    ASSERT_NE(ptr, nullptr);
    delete[] ptr;
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

TEST(AllocatorTest, NewDeleteNull)
{
    int* ptr = nullptr;
    delete ptr; // Should not crash
}

TEST(AllocatorTest, NewDeleteArrayNull)
{
    const int* ptr = nullptr;
    delete[] ptr; // Should not crash
}

TEST(AllocatorTest, NewDeleteMultiple)
{
    const auto ptr1 = new int(42);
    const auto ptr2 = new int(84);
    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_EQ(*ptr1, 42);
    ASSERT_EQ(*ptr2, 84);
    delete ptr1;
    delete ptr2;
}

TEST(AllocatorTest, NewDeleteArrayMultiple)
{
    const auto ptr1 = new int[50];
    const auto ptr2 = new int[100];
    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    delete[] ptr1;
    delete[] ptr2;
}