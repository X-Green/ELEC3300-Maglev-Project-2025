//
// Created by eeasee on 3/12/25.
//

#pragma once

#include "main.h"

namespace Tasks::ErrorChecker
{

const uint8_t ERROR_CODE_NO_POWER      = 0;
const uint8_t ERROR_CODE_OVER_VOLTAGE  = 1;
const uint8_t ERROR_CODE_SHORT_CIRCUIT = 2;
const uint8_t ERROR_CODE_OVER_CURRENT  = 3;
const uint8_t ERROR_CODE_OVER_TEMP     = 4;
const uint8_t ERROR_CODE_LEV_POSITION  = 5;
const uint8_t ERROR_CODE_LEV_EXISTENCE = 6;

const uint32_t ERROR_CODE_MASK_NO_POWER      = 1 << ERROR_CODE_NO_POWER;
const uint32_t ERROR_CODE_MASK_OVER_VOLTAGE  = 1 << ERROR_CODE_OVER_VOLTAGE;
const uint32_t ERROR_CODE_MASK_SHORT_CIRCUIT = 1 << ERROR_CODE_SHORT_CIRCUIT;
const uint32_t ERROR_CODE_MASK_OVER_CURRENT  = 1 << ERROR_CODE_OVER_CURRENT;
const uint32_t ERROR_CODE_MASK_OVER_TEMP     = 1 << ERROR_CODE_OVER_TEMP;
const uint32_t ERROR_CODE_MASK_LEV_POSITION  = 1 << ERROR_CODE_LEV_POSITION;
const uint32_t ERROR_CODE_MASK_LEV_EXISTENCE = 1 << ERROR_CODE_LEV_EXISTENCE;

inline uint32_t errorCode = ERROR_CODE_MASK_LEV_POSITION;

void updateErrorState();

void updateErrorStateFast();

}  // namespace Tasks::ErrorChecker