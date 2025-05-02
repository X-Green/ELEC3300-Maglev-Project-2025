//
// Created by eeasee on 4/2/25.
//

#pragma once

#include "main.h"
#include "tim.h"

namespace Tasks::WS2812Task
{

const uint8_t RGB_RED[3]    = {255, 0, 0};
const uint8_t RGB_ORANGE[3] = {255, 127, 0};
const uint8_t RGB_YELLOW[3] = {255, 255, 0};
const uint8_t RGB_GREEN[3]  = {0, 255, 0};
const uint8_t RGB_CYAN[3]   = {0, 255, 255};
const uint8_t RGB_BLUE[3]   = {0, 0, 255};
const uint8_t RGB_PURPLE[3] = {127, 0, 255};
const uint8_t RGB_WHITE[3]  = {255, 255, 255};
const uint8_t RGB_BLACK[3]  = {0, 0, 0};
const uint8_t RGB_PINK[3]   = {255, 0, 255};

const int LED_COUNT = 8;
inline uint8_t colors[LED_COUNT][3];  // R,G,B

void initWS2812();
void updateWS2812();

}  // namespace Tasks::WS2812Task