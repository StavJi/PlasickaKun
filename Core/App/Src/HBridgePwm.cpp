#include "HBridgePwm.hpp"
#include "cfg.hpp"

#include <algorithm>
#include "stm32g0xx_ll_tim.h"

HBridgePwm::HBridgePwm(std::uint32_t frequencyHz) {
    SetFrequencyHz(frequencyHz);
    LL_TIM_GenerateEvent_UPDATE(TIM1);
}

void HBridgePwm::Start(void) {
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N);
    LL_TIM_EnableAllOutputs(TIM1);
    LL_TIM_EnableCounter(TIM1);
}

void HBridgePwm::Stop(void) {
    LL_TIM_DisableCounter(TIM1);
    LL_TIM_DisableAllOutputs(TIM1);
    LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N);
}

void HBridgePwm::SetFrequencyHz(std::uint32_t frequencyHz) {
    frequencyHz = std::clamp(frequencyHz, config::FREQUENCY_MIN_HZ, config::FREQUENCY_MAX_HZ);
    if (frequencyHz == currentFrequencyHz_) {
        return;
    }

    const std::uint32_t period = config::TIM1_CLOCK_HZ / frequencyHz;
    // CubeMX enables ARR/CCR1 preload; writes take effect on an update event.
    LL_TIM_SetAutoReload(TIM1, period - 1U);
    LL_TIM_OC_SetCompareCH1(TIM1, period / 2U);
    currentFrequencyHz_ = frequencyHz;
}
