//
// Created by eeasee on 4/3/25.
//

#pragma once

#include "main.h"
#include "tim.h"

/**
 * Buzzer on TIM1 CH1 / PC3
 * 170MHz -(PreScaler=3400-1)-> 50kHz
 */

namespace Buzzer
{
/**
 * @brief Initialize the timer
 */
inline void init()
{
    htim1.Instance->CCR1 = 0U;
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

/**
 * @brief Play a tone
 *
 * @param frequency Frequency of the tone
 */
inline void play(uint32_t frequency)
{
    htim1.Instance->ARR  = (uint32_t)(50000U / frequency) - 1U;
    htim1.Instance->CCR1 = htim1.Instance->ARR / 2U;
    htim1.Instance->CNT  = 0U;
}

/**
 * @brief Play a tone
 *
 * @param frequency Frequency of the tone
 * @param durationMs Duration of the tone
 */
inline void play(uint32_t frequency, uint32_t durationMs)
{
    htim1.Instance->RCR  = (durationMs * frequency / 1000U) - 1U;
    htim1.Instance->EGR  = TIM_EGR_UG;
    htim1.Instance->ARR  = (50000U / frequency) - 1U;
    htim1.Instance->CCR1 = htim1.Instance->ARR / 2U;
    htim1.Instance->CNT  = 0U;
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
}

/**
 * @brief Stop playing the tone
 */
inline void stop() { htim1.Instance->CCR1 = 0U; }

/**
 * @brief When RCR triggered, means that the duration is over
 */
inline void onTIM1UpdateCallback()
{
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
    __HAL_TIM_SET_COUNTER(&htim1, 0U);
    stop();
}
}  // namespace Buzzer