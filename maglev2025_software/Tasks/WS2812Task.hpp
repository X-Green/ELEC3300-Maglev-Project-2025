//
// Created by eeasee on 4/2/25.
//

#pragma once

#include "main.h"
#include "tim.h"

namespace Tasks::WS2812Task
{

const uint8_t RED[3]    = {255, 0, 0};
const uint8_t ORANGE[3] = {255, 127, 0};
const uint8_t YELLOW[3] = {255, 255, 0};
const uint8_t GREEN[3]  = {0, 255, 0};
const uint8_t CYAN[3]   = {0, 255, 255};
const uint8_t BLUE[3]   = {0, 0, 255};
const uint8_t PURPLE[3] = {127, 0, 255};
const uint8_t WHITE[3]  = {255, 255, 255};
const uint8_t BLACK[3]  = {0, 0, 0};
const uint8_t PINK[3]   = {255, 0, 255};

const int LED_COUNT = 8;
inline uint8_t colors[LED_COUNT][3];  // R,G,B

void initWS2812();
void updateWS2812();

}  // namespace Tasks::WS2812Task