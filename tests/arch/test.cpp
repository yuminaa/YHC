#include <atomic>
#include <thread>
#include <vector>
#include <gtest/gtest.h>

#include "../../special/arch.hpp"

/**
 * @brief Test platform detection macros
 */
TEST(PlatformTest, DetectionMacros)
{
    #if defined(_WIN32) || defined(_WIN64)
        EXPECT_TRUE(true) << "Windows platform detected";
    #endif

    #if defined(__APPLE__) && defined(__MACH__)
        EXPECT_TRUE(true) << "macOS platform detected";
    #endif

    #if defined(__linux__)
        EXPECT_TRUE(true) << "Linux platform detected";
    #endif

    #if defined(YUMINA_OS_WINDOWS) || defined(YUMINA_OS_MACOS) || defined(YUMINA_OS_LINUX)
        EXPECT_TRUE(true) << "Platform detected correctly";
    #else
        FAIL() << "No platform detected";
    #endif
}

/**
 * @brief Test architecture detection
 */
TEST(ArchitectureTest, Detection)
{
    #if defined(YUMINA_ARCH_X64)
        EXPECT_TRUE(true) << "x64 architecture detected";
    #endif

    #if defined(YUMINA_ARCH_ARM64)
        EXPECT_TRUE(true) << "ARM64 architecture detected";
    #endif

    // At least one architecture should be detected
    #if defined(YUMINA_ARCH_X64) || defined(YUMINA_ARCH_ARM64)
        EXPECT_TRUE(true) << "Architecture detected correctly";
    #else
        FAIL() << "No architecture detected";
    #endif
}

/**
 * @brief Test fixture for SIMD operations
 */
class SIMDTest : public testing::Test
{
protected:
    static constexpr size_t TEST_SIZE = 1024;
    alignas(CACHE_LINE_SIZE) std::vector<float> data1;
    alignas(CACHE_LINE_SIZE) std::vector<float> data2;
    alignas(CACHE_LINE_SIZE) std::vector<float> result;

    void SetUp() override
    {
        data1.resize(TEST_SIZE);
        data2.resize(TEST_SIZE);
        result.resize(TEST_SIZE);

        for (std::size_t i = 0; i < TEST_SIZE; ++i)
        {
            data1[static_cast<std::vector<float>::size_type>(i)] = static_cast<float>(i);
            data2[static_cast<std::vector<float>::size_type>(i)] = static_cast<float>(i * 2);
        }
    }
};

TEST_F(SIMDTest, VectorAddition)
{
    for (size_t i = 0; i < TEST_SIZE; i += SIMD_WIDTH / sizeof(float))
    {
        #if defined(__AVX512F__)
                const auto vec1 = _mm512_loadu_ps(&data1[i]);
                const auto vec2 = _mm512_loadu_ps(&data2[i]);
                const auto sum = _mm512_add_ps(vec1, vec2);
                _mm512_storeu_ps(&result[i], sum);
        #elif defined(__AVX2__)
                const auto vec1 = _mm256_loadu_ps(&data1[i]);
                const auto vec2 = _mm256_loadu_ps(&data2[i]);
                const auto sum = _mm256_add_ps(vec1, vec2);
                _mm256_storeu_ps(&result[i], sum);
        #endif
    }

    for (std::vector<float>::size_type i = 0; i < TEST_SIZE; ++i)
    {
        EXPECT_FLOAT_EQ(result[i], data1[i] + data2[i]);
    }
}

/**
 * @brief Test fixture for memory operations
 */
class MemoryTest : public testing::Test
{
protected:
    void SetUp() override {}
};

TEST_F(MemoryTest, Fences)
{
    std::atomic flag{false};
    std::atomic value{0};

    std::thread producer([&value, &flag]
    {
        value.store(42, std::memory_order_relaxed);
        STORE_FENCE();
        flag.store(true, std::memory_order_release);
    });

    std::thread consumer([&flag, &value]
    {
        while (!flag.load(std::memory_order_acquire))
        {
            CPU_PAUSE();
        }
        LOAD_FENCE();
        EXPECT_EQ(value.load(std::memory_order_relaxed), 42);
    });

    producer.join();
    consumer.join();
}

TEST_F(MemoryTest, Alignment)
{
    struct CACHE_ALIGNED AlignedStruct
    {
        char data[100];
    };

    struct SIMD_ALIGNED SimdStruct
    {
        char data[100];
    };

    AlignedStruct cache_aligned_data{};
    SimdStruct simd_aligned_data{};

    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(&cache_aligned_data) % CACHE_LINE_SIZE, 0);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(&simd_aligned_data) % SIMD_WIDTH, 0);
}

TEST(OptimizationTest, BranchPrediction)
{
    auto sum = 0;
    for (auto i = 0; i < 1000; ++i)
    {
        if (LIKELY(i < 999))
        {
            sum += i;
        }
        if (UNLIKELY(i == 999))
        {
            sum += i;
        }
    }
    EXPECT_EQ(sum, 499500);
}

#if defined(YUMINA_ARCH_ARM64)
/**
 * @brief Test fixture for ARM-specific features
 */
class ARMTest : public testing::Test
{
protected:
    alignas(CACHE_LINE_SIZE) std::vector<uint8_t> test_data;

    void SetUp() override
    {
        test_data.resize(1024);
        std::fill(test_data.begin(), test_data.end(), 0xAA);
    }
};

TEST_F(ARMTest, CacheOperations)
{
    #ifdef __APPLE__
        GTEST_SKIP() << "Cache operations require special privileges on Apple Silicon";
    #else
        CACHE_CLEAN(&test_data[0]);
        CACHE_INVALIDATE(&test_data[0]);
        CACHE_CLEAN_AND_INVALIDATE(&test_data[0]);
        EXPECT_TRUE(true);
    #endif
}

TEST_F(ARMTest, BarrierOperations)
{
    DSB();
    DSB_ST();
    DSB_LD();
    ISB();
    EXPECT_TRUE(true);
}

#ifdef __ARM_NEON
TEST_F(ARMTest, NEONOperations)
{
    const float32x4_t a = vdupq_n_f32(1.0f);
    const float32x4_t b = vdupq_n_f32(2.0f);
    const float32x4_t c = vdupq_n_f32(3.0f);

    const float32x4_t result = vfmaq_f32(c, a, b);
    float results[4];
    vst1q_f32(results, result);

    for (const float value : results)
    {
        EXPECT_FLOAT_EQ(value, 5.0f);
    }
}
#endif
#endif

/**
 * @brief Main test runner
 */
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}