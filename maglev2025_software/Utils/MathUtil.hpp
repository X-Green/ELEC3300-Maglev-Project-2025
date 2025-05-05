//
// Created by xiaol on 2025/3/11.
//

#pragma once

#ifndef __STM32G474xx_H
#include "stm32g474xx.h"
#endif
#include "arm_math.h"

#define M_MAX(a, b) ((a) > (b) ? (a) : (b))

#define M_MIN(a, b) ((a) < (b) ? (a) : (b))

#define M_CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#define M_ABS(x) ((x) < 0 ? -(x) : (x))