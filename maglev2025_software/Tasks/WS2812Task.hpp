//
// Created by eeasee on 4/2/25.
//

#pragma once

#include "main.h"
#include "tim.h"

namespace Tasks::WS2812Task
{

const int LED_COUNT = 8;
inline uint8_t colors[LED_COUNT][3]; // R,G,B

void initWS2812();
void updateWS2812();

}  // namespace Tasks::WS2812Task