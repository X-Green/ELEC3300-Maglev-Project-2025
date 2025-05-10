//
// Created by eeasee on 3/12/25.
//

#pragma once

#include "main.h"

namespace Tasks::ErrorChecker
{

constexpr uint8_t ERROR_CODE_NO_POWER      = 0;
constexpr uint8_t ERROR_CODE_OVER_VOLTAGE  = 1;
constexpr uint8_t ERROR_CODE_SHORT_CIRCUIT = 2;
constexpr uint8_t ERROR_CODE_OVER_CURRENT  = 3;
constexpr uint8_t ERROR_CODE_OVER_TEMP     = 4;
constexpr uint8_t ERROR_CODE_LEV_POSITION  = 5;
constexpr uint8_t ERROR_CODE_LEV_EXISTENCE = 6;

constexpr uint32_t ERROR_CODE_MASK_NO_POWER      = 1 << ERROR_CODE_NO_POWER;
constexpr uint32_t ERROR_CODE_MASK_OVER_VOLTAGE  = 1 << ERROR_CODE_OVER_VOLTAGE;
constexpr uint32_t ERROR_CODE_MASK_SHORT_CIRCUIT = 1 << ERROR_CODE_SHORT_CIRCUIT;
constexpr uint32_t ERROR_CODE_MASK_OVER_CURRENT  = 1 << ERROR_CODE_OVER_CURRENT;
constexpr uint32_t ERROR_CODE_MASK_OVER_TEMP     = 1 << ERROR_CODE_OVER_TEMP;
constexpr uint32_t ERROR_CODE_MASK_LEV_POSITION  = 1 << ERROR_CODE_LEV_POSITION;
constexpr uint32_t ERROR_CODE_MASK_LEV_EXISTENCE = 1 << ERROR_CODE_LEV_EXISTENCE;

inline uint32_t errorCode = ERROR_CODE_MASK_LEV_POSITION;

void updateErrorState();

void updateErrorStateFast();

}  // namespace Tasks::ErrorChecker