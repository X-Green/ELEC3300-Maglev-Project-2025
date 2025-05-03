//
// Created by eeasee on 4/3/25.
//

#pragma once

#include "main.h"
#include "tim.h"

namespace Tasks::CommandInput
{
void initCommandInput();

inline uint32_t getEncoderValue() { return __HAL_TIM_GET_COUNTER(&htim8); }

inline uint8_t getVoiceCommandValue();

void onU1RXCallback();

void onVoiceCommandReceived(uint8_t command);
}  // namespace Tasks::CommandInput