//
// Created by eeasee on 4/2/25.
//

#pragma once

#include "main.h"
#include "tim.h"

namespace Drivers::WS2812
{

struct RGB_t
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

const RGB_t RGB_RED    = {255, 0, 0};
const RGB_t RGB_ORANGE = {255, 127, 0};
const RGB_t RGB_YELLOW = {255, 255, 0};
const RGB_t RGB_GREEN  = {0, 255, 0};
const RGB_t RGB_CYAN   = {0, 255, 255};
const RGB_t RGB_BLUE   = {0, 0, 255};
const RGB_t RGB_PURPLE = {127, 0, 255};
const RGB_t RGB_WHITE  = {255, 255, 255};
const RGB_t RGB_BLACK  = {0, 0, 0};
const RGB_t RGB_PINK   = {255, 0, 255};

const int LED_COUNT = 8;
inline RGB_t colors[LED_COUNT];  // R,G,B

void init();
void update();


}  // namespace Drivers::WS2812
