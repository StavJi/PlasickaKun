#include "ChirpGenerator.hpp"

#include "RepellerConfig.hpp"

ChirpGenerator::ChirpGenerator(HBridgePwm& pwm, std::uint32_t seed)
    : pwm_(pwm),
      rng_(seed),
      segmentStartMs_(0U),
      segmentDurationMs_(0U),
      segmentStartHz_(config::kMinFrequencyHz),
      segmentEndHz_(config::kMinFrequencyHz),
      lastUpdateMs_(0U),
      silent_(false),
      pwmRunning_(false) {
}

void ChirpGenerator::Begin(std::uint32_t nowMs) {
    lastUpdateMs_ = nowMs;
    StartNewSegment(nowMs);
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
    segmentStartHz_ = segmentEndHz_;  // continue from where the last sweep left off
    segmentEndHz_ = rng_.Range(config::kMinFrequencyHz, config::kMaxFrequencyHz);
}

std::uint32_t ChirpGenerator::CurrentFrequencyHz(std::uint32_t nowMs) const {
    const std::uint32_t elapsed = nowMs - segmentStartMs_;  // wraps correctly even at uint32 rollover
    if (elapsed >= segmentDurationMs_ || segmentDurationMs_ == 0U) {
        return segmentEndHz_;
    }

    // Linear interpolation between start and end frequency over the
    // segment's duration.
    const std::int64_t start = static_cast<std::int64_t>(segmentStartHz_);
    const std::int64_t end = static_cast<std::int64_t>(segmentEndHz_);
    const std::int64_t delta = end - start;
    const std::int64_t f = start + (delta * static_cast<std::int64_t>(elapsed)) /
                                        static_cast<std::int64_t>(segmentDurationMs_);
    return static_cast<std::uint32_t>(f);
}

void ChirpGenerator::Update(std::uint32_t nowMs)
{
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
        if (!pwmRunning_) {
            pwm_.Start();
            pwmRunning_ = true;
        }
        pwm_.SetFrequencyHz(CurrentFrequencyHz(nowMs));
    }
}
