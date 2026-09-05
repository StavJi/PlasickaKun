#pragma once

#include <cstdint>

// Compile-time (constexpr) computation of the TIM1 BDTR.DTG dead-time byte.
//
//   DTG[7:5] = 0xx  ->  DT = DTG[7:0]        * 1  * tDTS   (0   ..  127 * tDTS)
//   DTG[7:5] = 10x  ->  DT = (64 + DTG[5:0])  * 2  * tDTS   (128 ..  254 * tDTS)
//   DTG[7:5] = 110  ->  DT = (32 + DTG[4:0])  * 8  * tDTS   (256 ..  504 * tDTS)
//   DTG[7:5] = 111  ->  DT = (32 + DTG[4:0])  * 16 * tDTS   (512 .. 1008 * tDTS)
//
// This fixes the dead time at compile time (a `constexpr`), per
// design choice: it is not meant to be re-tuned at runtime, only by
// changing App/Inc/RepellerConfig.hpp and reflashing.
namespace deadtime {

// Picks the coarsest-resolution range that can still represent the
// requested dead time, then rounds to the nearest representable step
// within that range (never exceeding the 1008*tDTS absolute maximum).
constexpr std::uint8_t ComputeDtg(double deadTimeSeconds, double timerClockHz, std::uint32_t ckd = 0)
{
    // tDTS derives from the timer's internal clock (CK_INT) via CR1.CKD.
    const double tCkInt = 1.0 / timerClockHz;
    const double tDts = tCkInt * static_cast<double>(ckd == 0U ? 1U : (ckd == 1U ? 2U : 4U));

    if (deadTimeSeconds <= 0.0) {
        return 0U;
    }

    // Range 1: finest resolution (1 * tDTS per step), covers 0 .. 127 steps.
    if (deadTimeSeconds <= 127.0 * tDts) {
        auto v = static_cast<std::uint32_t>(deadTimeSeconds / tDts + 0.5);
        if (v > 127U) {
            v = 127U;
        }
        return static_cast<std::uint8_t>(v);
    }

    // Range 2: 2 * tDTS per step, covers 128 .. 254 * tDTS.
    if (deadTimeSeconds <= 127.0 * 2.0 * tDts) {
        auto v = static_cast<std::uint32_t>(deadTimeSeconds / (2.0 * tDts) + 0.5);
        if (v < 64U) {
            v = 64U;
        }
        if (v > 127U) {
            v = 127U;
        }
        return static_cast<std::uint8_t>(0x80U | (v - 64U));
    }

    // Range 3: 8 * tDTS per step, covers 256 .. 504 * tDTS.
    if (deadTimeSeconds <= 63.0 * 8.0 * tDts) {
        auto v = static_cast<std::uint32_t>(deadTimeSeconds / (8.0 * tDts) + 0.5);
        if (v < 32U) {
            v = 32U;
        }
        if (v > 63U) {
            v = 63U;
        }
        return static_cast<std::uint8_t>(0xC0U | (v - 32U));
    }

    // Range 4: 16 * tDTS per step, covers 512 .. 1008 * tDTS (the maximum
    // dead time TIM1's BDTR.DTG can express). Values requesting more than
    // this are clamped -- widen the gate drive margin in hardware instead.
    {
        auto v = static_cast<std::uint32_t>(deadTimeSeconds / (16.0 * tDts) + 0.5);
        if (v < 32U) {
            v = 32U;
        }
        if (v > 63U) {
            v = 63U;
        }
        return static_cast<std::uint8_t>(0xE0U | (v - 32U));
    }
}

// Returns the actual dead time (in seconds) a given DTG byte encodes
// useful for a `static_assert` sanity check against the requested value.
constexpr double DtgToSeconds(std::uint8_t dtg, double timerClockHz, std::uint32_t ckd = 0)
{
    const double tCkInt = 1.0 / timerClockHz;
    const double tDts = tCkInt * static_cast<double>(ckd == 0U ? 1U : (ckd == 1U ? 2U : 4U));

    if ((dtg & 0x80U) == 0U) {
        return static_cast<double>(dtg) * tDts;
    }
    if ((dtg & 0x40U) == 0U) {
        return static_cast<double>(64U + (dtg & 0x3FU)) * 2.0 * tDts;
    }
    if ((dtg & 0x20U) == 0U) {
        return static_cast<double>(32U + (dtg & 0x1FU)) * 8.0 * tDts;
    }
    return static_cast<double>(32U + (dtg & 0x1FU)) * 16.0 * tDts;
}

}  // namespace deadtime
