#pragma once

#include "arch.hpp"

ALWAYS_INLINE static void yumina_fast_copy(void* dst, const void* src, size_t size)
{
    #if defined(YUMINA_ARCH_X64)
        if (size >= 32)
        {
            auto* d = static_cast<char*>(dst);
            auto* s = static_cast<const char*>(src);

    #ifdef __AVX512F__
            for (; size >= 64; size -= 64, s += 64, d += 64)
            {
                __m512i v = _mm512_loadu_si512((__m512i*)s);
                _mm512_storeu_si512((__m512i*)d, v);
            }
    #endif

    #ifdef __AVX2__
            for (; size >= 32; size -= 32, s += 32, d += 32)
            {
                __m256i v = _mm256_loadu_si256((__m256i*)s);
                _mm256_storeu_si256((__m256i*)d, v);
            }
    #endif
            if (size > 0)
            {
                std::memcpy(d, s, size);
            }
        }
        else
        {
            std::memcpy(dst, src, size);
        }
    #elif defined(YUMINA_ARCH_ARM64)
        if (size >= 16)
        {
            auto* d = static_cast<char*>(dst);
            auto* s = static_cast<const char*>(src);

            for (; size >= 16; size -= 16, s += 16, d += 16)
            {
                uint8x16_t v = vld1q_u8(reinterpret_cast<const uint8_t *>(s));
                vst1q_u8(reinterpret_cast<uint8_t *>(d), v);
            }

            if (size > 0)
            {
                std::memcpy(d, s, size);
            }
        }
        else
        {
            std::memcpy(dst, src, size);
        }
    #else
        std::memcpy(dst, src, size);
    #endif
}