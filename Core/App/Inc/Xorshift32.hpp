#pragma once

#include <cstdint>

// Minimal xorshift32 PRNG with zero flash/RAM overhead and no hardware
class Xorshift32 {
public:
    explicit Xorshift32(std::uint32_t seed) : state_(seed != 0U ? seed : 0xA5A5A5A5U) {
    }

    std::uint32_t Next(void) {
        std::uint32_t x = state_;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        state_ = x;
        return x;
    }

    // maxInclusive must be >= minInclusive
    std::uint32_t Range(std::uint32_t minInclusive, std::uint32_t maxInclusive) {
        if (maxInclusive <= minInclusive) {
            return minInclusive;
        }
        const std::uint32_t span = maxInclusive - minInclusive + 1U;
        return minInclusive + (Next() % span);
    }

private:
    std::uint32_t state_;
};
