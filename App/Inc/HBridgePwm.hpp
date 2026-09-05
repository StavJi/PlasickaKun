#pragma once

#include <cstdint>

// Drives a full H-bridge from TIM1's complementary channel 1:
//
//              +-----+                    +-----+
//        VBUS--| Q_A |--*--( transducer )--| Q_B |--VBUS
//              +-----+  |                  +-----+
//   TIM1_CH1  --------> | high-side A                | high-side B <-- TIM1_CH1N
//                       |                             |
//              +-----+  |                    +-----+  |
//         GND--| Q_C |--*                    | Q_D |--*--GND
//              +-----+                       +-----+
//   TIM1_CH1  --------> low-side D (diagonal w/ high-side A's complement... )
//
// Concretely: TIM1_CH1 (PA8) drives the gate signal shared by diagonal 1
// (high-side A + low-side D), and TIM1_CH1N (PA7) drives the complementary
// signal shared by diagonal 2 (high-side B + low-side C). TIM1's hardware
// dead-time generator (BDTR.DTG, see App/Inc/DeadTime.hpp) guarantees a
// fixed gap between one diagonal turning off and the other turning on, so
// the two diagonals can never conduct at the same time regardless of the
// PWM frequency in use.
//
// External gate-drive/level-shifting hardware between these two MCU pins
// and the actual H-bridge FETs is assumed (e.g. two half-bridge gate
// drivers, one per leg, each driven by one of these two signals) -- this
// class only produces the two complementary, dead-time-separated logic
// level PWM signals.
//
// NOTE ON PINS: TIM1_CH1/CH1N = PA8/PA7 is the standard AF2 assignment for
// the whole STM32G0 family. Double-check PA8/PA7 against the "Pin
// assignment" table (and TSSOP20 pinout figure) in the STM32G031x4/x6/x8
// datasheet (DS12992) before finalizing your PCB -- this project targets
// STM32G031F4P6 (TSSOP20) but the exact physical pin *numbers* for PA7/PA8
// on that specific package were not independently re-verified here.
class HBridgePwm {
public:
    // Call MX_TIM1_Init() first. CubeMX owns GPIO, PWM mode and dead time.
    // The constructor sets only the initial frequency and loads the preloads.
    explicit HBridgePwm(std::uint32_t frequencyHz);

    HBridgePwm(const HBridgePwm&) = delete;
    HBridgePwm& operator=(const HBridgePwm&) = delete;

    // Enables the timer outputs (MOE) and starts the counter. Both
    // complementary outputs idle low (via OSSI/OSSR) until this is called.
    void Start();

    // Forces both outputs to their safe idle (low) state and stops the
    // counter. Safe to call at any time, including from a fault path.
    void Stop();

    // Changes only TIM1's auto-reload/compare registers (via the shadow/
    // preload registers, applied atomically on the next update event) to
    // retain the 50% duty cycle at the new period. The absolute dead time
    // (in seconds) is unaffected because BDTR.DTG counts ticks of the
    // timer's input clock, not of the (changing) PWM period.
    void SetFrequencyHz(std::uint32_t frequencyHz);

private:
    void ApplyFrequency(std::uint32_t frequencyHz);

    std::uint32_t currentFrequencyHz_;
};
