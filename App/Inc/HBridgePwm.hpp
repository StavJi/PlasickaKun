#pragma once

#include <cstdint>

class HBridgePwm {
public:
    // Call MX_TIM1_Init() first. CubeMX owns GPIO, PWM mode and dead time.
    // The constructor sets only the initial frequency and loads the preloads.
    explicit HBridgePwm(std::uint32_t frequencyHz);

    HBridgePwm(const HBridgePwm&) = delete;
    HBridgePwm& operator=(const HBridgePwm&) = delete;

    // Enables the timer outputs (MOE) and starts the counter. Both
    // complementary outputs idle low (via OSSI/OSSR) until this is called.
    void Start(void);

    // Forces both outputs to their safe idle (low) state and stops the
    // counter. Safe to call at any time, including from a fault path.
    void Stop(void);

    // Changes only TIM1 auto-reload/compare registers (via the shadow/
    // preload registers, applied atomically on the next update event) to
    // retain the 50% duty cycle at the new period. The absolute dead time
    // (in seconds) is unaffected because BDTR.DTG counts ticks of the
    // timer's input clock, not of the (changing) PWM period.
    void SetFrequencyHz(std::uint32_t frequencyHz);

private:
    void ApplyFrequency(std::uint32_t frequencyHz);

    std::uint32_t currentFrequencyHz_;
};
