#include <gtest/gtest.h>
#include <memory/allocator.h>

using namespace yumina::detail::yumina::detail::internal;

class AllocatorTest : public testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(AllocatorTest, BasicAllocation)
{
    void* ptr = allocate(5);
    ASSERT_NE(ptr, nullptr) << "Allocation failed; returned a null pointer";
    deallocate(ptr);
}

TEST_F(AllocatorTest, AllocationDifferentSizes)
{
    for (const std::size_t sizes[] = {1, 16, 64, 128, 512, 1024}; const std::size_t size : sizes)
    {
        void* ptr = allocate(size);
        ASSERT_NE(ptr, nullptr) << "Allocation failed for size: " << size;
        deallocate(ptr);
    }
}

TEST_F(AllocatorTest, AllocationAlignment)
{
    constexpr std::size_t size = 64;
    void* ptr = allocate(size);
    ASSERT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % alignof(std::max_align_t), 0)
        << "Pointer is not aligned as expected";
    deallocate(ptr);
}

TEST_F(AllocatorTest, LargeAllocationFailure)
{
    constexpr auto very_large_size = static_cast<std::size_t>(-1);
    void* ptr = allocate(very_large_size);
    ASSERT_EQ(ptr, nullptr) << "Allocation should fail for an unrealistically large size";
}
