#pragma once

#include <cstdint>

#include "DeadTime.hpp"

// Central, compile-time configuration for the marten repeller.
//
// Everything here is a `constexpr`: nothing is meant to be tuned at
// runtime (no potentiometer/ADC, no UART command) -- change a value and
// reflash, as requested.
namespace config {

// ---------------------------------------------------------------------
// Clocks
// ---------------------------------------------------------------------
// SYSCLK = HCLK = PCLK = 64 MHz (HSI16 -> PLL x8 /2, see SystemClock_Config
// in CubeMX-generated main.c). TIM1 uses PCLK with no additional multiplier
// applied when the APB prescaler is 1, so TIM1's counter clock (CK_INT) is
// also 64 MHz.
inline constexpr std::uint32_t kSysClockHz = 64'000'000U;
inline constexpr std::uint32_t kTim1ClockHz = kSysClockHz;

// ---------------------------------------------------------------------
// H-bridge dead time (fixed, compile-time constant)
// ---------------------------------------------------------------------
// Gap enforced by TIM1's hardware dead-time generator between one
// diagonal's PWM (TIM1_CH1) turning off and the other diagonal's PWM
// (TIM1_CH1N) turning on, and vice versa. This must be set comfortably
// above the *sum* of your MOSFET/driver turn-off delay + turn-on delay
// (check your driver IC / FET gate charge and driver current) -- 500 ns
// is a reasonable starting point for small-signal MOSFETs with a decent
// gate driver, but re-check against your actual switches before relying
// on it.
// Reference values only: configure TIM1 Dead Time in PlasickaKun.ioc.
// CubeMX currently generates DTG = 32 at 64 MHz, i.e. 500 ns.
inline constexpr double kDeadTimeSeconds = 500.0e-9;  // 500 ns

inline constexpr std::uint8_t kDeadTimeDtg =
    deadtime::ComputeDtg(kDeadTimeSeconds, static_cast<double>(kTim1ClockHz));

// Sanity check: make sure the achievable dead time is within +/-1 tick of
// what was requested (it will be, as long as it's representable at all in
// one of the four DTG ranges -- this mainly guards against asking for more
// than the ~1008 * tDTS hardware maximum, which would silently clamp).
static_assert(deadtime::DtgToSeconds(kDeadTimeDtg, static_cast<double>(kTim1ClockHz)) <
                  kDeadTimeSeconds + 5.0e-9,
              "Requested dead time exceeds TIM1's hardware maximum (~1008 timer "
              "clock cycles) and was clamped -- lower kDeadTimeSeconds, lower "
              "kTim1ClockHz, or accept the clamp.");

// ---------------------------------------------------------------------
// Output frequency sweep ("rozmitany" chirp) range
// ---------------------------------------------------------------------
// With PSC = 0 (no prescaler) TIM1's 16-bit ARR limits how *low* the
// frequency can go (ARR = kTim1ClockHz / f - 1 must fit in 16 bits), and
// practical dead-time-to-period ratio limits how *high* it should go.
// Nominal output range annotated in the reference repeller schematic.
inline constexpr std::uint32_t kMinFrequencyHz = 30'000U;  // 30 kHz
inline constexpr std::uint32_t kMaxFrequencyHz = 40'000U;  // 40 kHz

static_assert(kMinFrequencyHz > 0U, "Frequency must be positive.");
static_assert(kMaxFrequencyHz > kMinFrequencyHz, "Max must exceed min frequency.");
static_assert(kTim1ClockHz / kMinFrequencyHz - 1U <= 0xFFFFU,
              "kMinFrequencyHz is too low for a 16-bit ARR at this timer clock "
              "-- raise kMinFrequencyHz or add a TIM1 prescaler.");

// Hold a random frequency for a random duration, then select a new tone.
// Durations are a software choice, not measured from the reference circuit.
inline constexpr std::uint32_t kMinSegmentDurationMs = 150U;
inline constexpr std::uint32_t kMaxSegmentDurationMs = 900U;

inline constexpr std::uint32_t kMinSilenceMs = 0U;     // set > 0 for gaps between chirps
inline constexpr std::uint32_t kMaxSilenceMs = 0U;

// How often the chirp generator recomputes the instantaneous frequency
// and writes it to TIM1's ARR/CCR1. 1 kHz (every SysTick ms) is more than
// smooth enough for an audio/ultrasonic sweep and keeps main-loop CPU load
// negligible.
inline constexpr std::uint32_t kUpdatePeriodMs = 1U;

}  // namespace config
