//
// Created by xiaol on 2025/3/11.
//

#pragma once

#include "stm32g474xx.h"
//Order is important here
#include "arm_math.h"


#define M_MAX(a, b) ((a) > (b) ? (a) : (b))
#define M_MIN(a, b) ((a) < (b) ? (a) : (b))
#define M_CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))