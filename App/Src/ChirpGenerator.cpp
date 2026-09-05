#include "ChirpGenerator.hpp"

#include "RepellerConfig.hpp"

ChirpGenerator::ChirpGenerator(HBridgePwm& pwm, std::uint32_t seed)
    : pwm_(pwm),
      rng_(seed),
      segmentStartMs_(0U),
      segmentDurationMs_(0U),

      segmentEndHz_(config::kMinFrequencyHz),
      lastUpdateMs_(0U),
      silent_(false),
      pwmRunning_(false) {
}

void ChirpGenerator::Begin(std::uint32_t nowMs) {
    lastUpdateMs_ = nowMs;
    StartNewSegment(nowMs);
    if (!silent_) {
        pwm_.SetFrequencyHz(CurrentFrequencyHz());
        pwm_.Start();
        pwmRunning_ = true;
    }
}

void ChirpGenerator::StartNewSegment(std::uint32_t nowMs) {
    const bool haveSilenceGap = config::kMaxSilenceMs > 0U;
    silent_ = haveSilenceGap && (rng_.Range(0U, 1U) == 0U);

    if (silent_) {
        segmentStartMs_ = nowMs;
        segmentDurationMs_ = rng_.Range(config::kMinSilenceMs, config::kMaxSilenceMs);
        return;
    }

    segmentStartMs_ = nowMs;
    segmentDurationMs_ = rng_.Range(config::kMinSegmentDurationMs, config::kMaxSegmentDurationMs);
    // Hold one random frequency for the whole segment.
    segmentEndHz_ = rng_.Range(config::kMinFrequencyHz, config::kMaxFrequencyHz);
}

std::uint32_t ChirpGenerator::CurrentFrequencyHz() const {
    return segmentEndHz_;
}

void ChirpGenerator::Update(std::uint32_t nowMs) {
    if ((nowMs - lastUpdateMs_) < config::kUpdatePeriodMs) {
        return;
    }
    
    lastUpdateMs_ = nowMs;

    const std::uint32_t elapsed = nowMs - segmentStartMs_;
    if (elapsed >= segmentDurationMs_) {
        StartNewSegment(nowMs);
    }

    if (silent_) {
        if (pwmRunning_) {
            pwm_.Stop();
            pwmRunning_ = false;
        }
    } else {
        pwm_.SetFrequencyHz(CurrentFrequencyHz());
        if (!pwmRunning_) {
            pwm_.Start();
            pwmRunning_ = true;
        }
    }
}

