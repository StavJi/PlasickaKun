#pragma once

#include <cstdint>

class HBridgePwm {
    public:
        // MX_TIM1_Init() must run first; it configures GPIO and dead time.
        explicit HBridgePwm(std::uint32_t frequencyHz);

        HBridgePwm(const HBridgePwm&) = delete;
        HBridgePwm& operator=(const HBridgePwm&) = delete;

        void Start(void);
        void Stop(void);
        void SetFrequencyHz(std::uint32_t frequencyHz);

    private:
        std::uint32_t currentFrequencyHz_ = 0;
};
