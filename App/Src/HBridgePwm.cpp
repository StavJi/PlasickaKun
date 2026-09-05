#include "HBridgePwm.hpp"



#include "stm32g0xx_ll_tim.h"

#include "RepellerConfig.hpp"

namespace {

std::uint32_t ComputeArr(std::uint32_t frequencyHz) {
    if (frequencyHz < config::kMinFrequencyHz) {
        frequencyHz = config::kMinFrequencyHz;
    }

    if (frequencyHz > config::kMaxFrequencyHz) {
        frequencyHz = config::kMaxFrequencyHz;
    }
    // PSC = 0, so TIM1 counter clock equals config::kTim1ClockHz.
    return (config::kTim1ClockHz / frequencyHz) - 1U;
}

}  // namespace

HBridgePwm::HBridgePwm(std::uint32_t frequencyHz) 
    : currentFrequencyHz_(frequencyHz) {

    // CubeMX configures GPIO, PWM mode, preload and dead time before this constructor.
    ApplyFrequency(frequencyHz);
    LL_TIM_GenerateEvent_UPDATE(TIM1);
}

void HBridgePwm::Start(void) {
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N);
    LL_TIM_EnableAllOutputs(TIM1);
    LL_TIM_EnableCounter(TIM1);
}

void HBridgePwm::Stop() {
    LL_TIM_DisableCounter(TIM1);
    LL_TIM_DisableAllOutputs(TIM1);
    LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N);
}

void HBridgePwm::SetFrequencyHz(std::uint32_t frequencyHz) {
    if (frequencyHz == currentFrequencyHz_) {
        return;
    }
    ApplyFrequency(frequencyHz);
    currentFrequencyHz_ = frequencyHz;
}

void HBridgePwm::ApplyFrequency(std::uint32_t frequencyHz) {
    const std::uint32_t arr = ComputeArr(frequencyHz);
    // Both ARR and CCR1 are buffered (preload enabled above), so this pair
    // of writes takes effect atomically on the next update event
    LL_TIM_SetAutoReload(TIM1, arr);
    LL_TIM_OC_SetCompareCH1(TIM1, (arr + 1U) / 2U);
}

