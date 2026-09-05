#pragma once

#include <cstdint>

#include "HBridgePwm.hpp"
#include "Xorshift32.hpp"

// This class is non-blocking: call Update(nowMs) periodically (e.g. once
// per millisecond from the main loop, driven off the SysTick-based
// millisecond counter) and it will advance the sweep and push new
// frequencies to the HBridgePwm as needed. It never blocks or delays.
class ChirpGenerator {
public:
    ChirpGenerator(HBridgePwm& pwm, std::uint32_t seed);
    void Begin(std::uint32_t nowMs);
    void Update(std::uint32_t nowMs);

private:
    void StartNewSegment(std::uint32_t nowMs);
    std::uint32_t CurrentFrequencyHz(std::uint32_t nowMs) const;

    HBridgePwm& pwm_;
    Xorshift32 rng_;

    std::uint32_t segmentStartMs_;
    std::uint32_t segmentDurationMs_;
    std::uint32_t segmentStartHz_;
    std::uint32_t segmentEndHz_;
    std::uint32_t lastUpdateMs_;
    bool silent_;
    bool pwmRunning_;
};
