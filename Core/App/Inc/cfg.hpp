#pragma once

#include <cstdint>
namespace config {
    static constexpr uint32_t UPDATE_PERIOD_MS = 1;
    static constexpr uint32_t FREQUENCY_MIN_HZ = 30'000;
    static constexpr uint32_t FREQUENCY_MAX_HZ = 40'000;
    static constexpr uint32_t SILENCE_MIN_MS = 0;
    static constexpr uint32_t SILENCE_MAX_MS = 0;
    static constexpr uint32_t SEGMENT_DURATION_MIN_MS = 150;
    static constexpr uint32_t SEGMENT_DURATION_MAX_MS = 900;

    static constexpr uint32_t SYS_CLOCK_HZ = 64'000'000;
    static constexpr uint32_t TIM1_CLOCK_HZ = SYS_CLOCK_HZ;
}
