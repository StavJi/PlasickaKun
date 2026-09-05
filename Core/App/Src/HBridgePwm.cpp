#include "HBridgePwm.hpp"

#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_tim.h"

#include "RepellerConfig.hpp"

namespace {

// PA8 = TIM1_CH1 (AF2), PA7 = TIM1_CH1N (AF2) -- see the pin note in
// HBridgePwm.hpp.
constexpr std::uint32_t kChPort = LL_GPIO_PIN_8;   // PA8, TIM1_CH1
constexpr std::uint32_t kChnPort = LL_GPIO_PIN_7;  // PA7, TIM1_CH1N

void InitGpio()
{
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    LL_GPIO_InitTypeDef gpioInit{};
    gpioInit.Pin = kChPort | kChnPort;
    gpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    gpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpioInit.Pull = LL_GPIO_PULL_NO;
    gpioInit.Alternate = LL_GPIO_AF_2;
    LL_GPIO_Init(GPIOA, &gpioInit);
}

std::uint32_t ComputeArr(std::uint32_t frequencyHz)
{
    if (frequencyHz < config::kMinFrequencyHz) {
        frequencyHz = config::kMinFrequencyHz;
    }
    if (frequencyHz > config::kMaxFrequencyHz) {
        frequencyHz = config::kMaxFrequencyHz;
    }
    // PSC = 0, so TIM1's counter clock equals config::kTim1ClockHz.
    return (config::kTim1ClockHz / frequencyHz) - 1U;
}

}  // namespace

HBridgePwm::HBridgePwm(std::uint32_t frequencyHz) : currentFrequencyHz_(frequencyHz)
{
    InitGpio();

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

    const std::uint32_t arr = ComputeArr(frequencyHz);

    LL_TIM_InitTypeDef timInit{};
    timInit.Prescaler = 0U;
    timInit.CounterMode = LL_TIM_COUNTERMODE_UP;
    timInit.Autoreload = arr;
    timInit.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    timInit.RepetitionCounter = 0U;
    LL_TIM_Init(TIM1, &timInit);

    // 50% duty cycle: CH1 high for the first half of the period, CH1N
    // (after the dead-time gap) high for the second half.
    LL_TIM_OC_InitTypeDef ocInit{};
    ocInit.OCMode = LL_TIM_OCMODE_PWM1;
    ocInit.OCState = LL_TIM_OCSTATE_ENABLE;
    ocInit.OCNState = LL_TIM_OCSTATE_ENABLE;
    ocInit.CompareValue = (arr + 1U) / 2U;
    ocInit.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    ocInit.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
    ocInit.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
    ocInit.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH1, &ocInit);
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH1);

    // Hardware dead-time / break configuration. Break inputs are unused
    // (no external fault comparator wired in this design) -- MOE is
    // controlled purely by software via Start()/Stop().
    LL_TIM_BDTR_InitTypeDef bdtrInit{};
    bdtrInit.OSSRState = LL_TIM_OSSR_ENABLE;
    bdtrInit.OSSIState = LL_TIM_OSSI_ENABLE;
    bdtrInit.LockLevel = LL_TIM_LOCKLEVEL_OFF;
    bdtrInit.DeadTime = config::kDeadTimeDtg;
    bdtrInit.BreakState = LL_TIM_BREAK_DISABLE;
    bdtrInit.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
    bdtrInit.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
    bdtrInit.BreakAFMode = LL_TIM_BREAK_AFMODE_INPUT;
    bdtrInit.Break2State = LL_TIM_BREAK2_DISABLE;
    bdtrInit.Break2Polarity = LL_TIM_BREAK_POLARITY_HIGH;
    bdtrInit.Break2Filter = LL_TIM_BREAK_FILTER_FDIV1;
    bdtrInit.Break2AFMode = LL_TIM_BREAK_AFMODE_INPUT;
    bdtrInit.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
    LL_TIM_BDTR_Init(TIM1, &bdtrInit);

    LL_TIM_EnableARRPreload(TIM1);
    LL_TIM_GenerateEvent_UPDATE(TIM1);
}

void HBridgePwm::Start()
{
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N);
    LL_TIM_EnableAllOutputs(TIM1);
    LL_TIM_EnableCounter(TIM1);
}

void HBridgePwm::Stop()
{
    LL_TIM_DisableCounter(TIM1);
    LL_TIM_DisableAllOutputs(TIM1);
    LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N);
}

void HBridgePwm::SetFrequencyHz(std::uint32_t frequencyHz)
{
    if (frequencyHz == currentFrequencyHz_) {
        return;
    }
    ApplyFrequency(frequencyHz);
    currentFrequencyHz_ = frequencyHz;
}

void HBridgePwm::ApplyFrequency(std::uint32_t frequencyHz)
{
    const std::uint32_t arr = ComputeArr(frequencyHz);
    // Both ARR and CCR1 are buffered (preload enabled above), so this pair
    // of writes takes effect atomically on the next update event -- no
    // torn/glitched period while the frequency is swept.
    LL_TIM_SetAutoReload(TIM1, arr);
    LL_TIM_OC_SetCompareCH1(TIM1, (arr + 1U) / 2U);
}
