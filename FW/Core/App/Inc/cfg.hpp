#pragma once

#include <cstdint>

namespace config {
    inline constexpr std::uint32_t FREQUENCY_MIN_HZ = 30'000;
    inline constexpr std::uint32_t FREQUENCY_MAX_HZ = 40'000;
    inline constexpr std::uint32_t SEGMENT_DURATION_MIN_MS = 150;
    inline constexpr std::uint32_t SEGMENT_DURATION_MAX_MS = 900;

    // Must match CubeMX: TIM1 clock 64 MHz, prescaler 0.
    inline constexpr std::uint32_t TIM1_CLOCK_HZ = 64'000'000;

    static_assert(FREQUENCY_MIN_HZ > 0 && FREQUENCY_MIN_HZ <= FREQUENCY_MAX_HZ);
    static_assert(TIM1_CLOCK_HZ / FREQUENCY_MIN_HZ <= 65536);
    static_assert(FREQUENCY_MAX_HZ <= TIM1_CLOCK_HZ / 2);
    static_assert(SEGMENT_DURATION_MIN_MS > 0 && SEGMENT_DURATION_MIN_MS <= SEGMENT_DURATION_MAX_MS);
}
