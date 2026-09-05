#include "ChirpGenerator.hpp"
#include "cfg.hpp"

ChirpGenerator::ChirpGenerator(HBridgePwm& pwm, std::uint32_t seed)
    : pwm_{pwm}
    , rng_{seed} {
}

void ChirpGenerator::Begin(std::uint32_t nowMs) {
    NextTone(nowMs);
    pwm_.Start();
}

void ChirpGenerator::Update(std::uint32_t nowMs) {
    // Unsigned subtraction also handles tick counter rollover.
    if (nowMs - toneStartMs_ >= toneDurationMs_) {
        NextTone(nowMs);
    }
}

void ChirpGenerator::NextTone(std::uint32_t nowMs) {
    toneStartMs_ = nowMs;
    toneDurationMs_ = rng_.Range(config::SEGMENT_DURATION_MIN_MS, config::SEGMENT_DURATION_MAX_MS);
    pwm_.SetFrequencyHz(rng_.Range(config::FREQUENCY_MIN_HZ, config::FREQUENCY_MAX_HZ));
}
