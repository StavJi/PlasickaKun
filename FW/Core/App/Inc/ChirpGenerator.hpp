#pragma once

#include <cstdint>

#include "HBridgePwm.hpp"
#include "Xorshift32.hpp"

class ChirpGenerator {
    public:
        ChirpGenerator(HBridgePwm& pwm, std::uint32_t seed);
        void Begin(std::uint32_t nowMs);
        void Update(std::uint32_t nowMs);

    private:
        void NextTone(std::uint32_t nowMs);

        HBridgePwm& pwm_;
        Xorshift32 rng_;
        std::uint32_t toneStartMs_ = 0;
        std::uint32_t toneDurationMs_ = 0;
};
