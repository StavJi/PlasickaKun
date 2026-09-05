#pragma once

#include <cstdint>

#include "HBridgePwm.hpp"
#include "Xorshift32.hpp"

// Produces the "rozmitany" (swept) tone: a sequence of chirp segments,
// each linearly ramping between two randomly chosen frequencies over a
// randomly chosen duration, continuously driving an HBridgePwm. The
// varying frequency/duration is what keeps a marten from habituating to a
// single, predictable tone.
//
// This class is non-blocking: call Update(nowMs) periodically (e.g. once
// per millisecond from the main loop, driven off the SysTick-based
// millisecond counter) and it will advance the sweep and push new
// frequencies to the HBridgePwm as needed. It never blocks or delays.
class ChirpGenerator {
public:
    // `pwm` must outlive this object. `seed` should differ between boots
    // (e.g. derived from an unconnected/floating ADC channel sampled at
    // startup) so the sweep pattern isn't identical after every reset.
    ChirpGenerator(HBridgePwm& pwm, std::uint32_t seed);

    // Call once, after HBridgePwm::Start(), to begin the first segment.
    void Begin(std::uint32_t nowMs);

    // Call frequently (>= the configured update period) with the current
    // millisecond timestamp; advances/starts segments as needed.
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
