//
// Created by eeasee on 5/2/25.
//

#include "WS2812.hpp"

const uint32_t BUFFER_SIZE  = Drivers::WS2812::LED_COUNT * 24 + 100;  // with reset pulse
uint8_t buffer[BUFFER_SIZE] = {};

/**
 * WS2812 Use Timer2 Channel2; Prescaler=71-1; ARR=3-1
 * 0-Code is [1,0,0] -> Pulse=1
 * 1-Code is [1,1,0] -> Pulse=2
 */

const uint8_t COMPARE_CODE_0 = 0x01;
const uint8_t COMPARE_CODE_1 = 0x02;

void updateBuffer()
{
    uint32_t buffer_index = 0;
    for (const auto &color : Drivers::WS2812::colors)
    {
        // Green, Red, Blue order for 2812
        uint32_t red   = color.r;
        uint32_t green = color.g;
        uint32_t blue  = color.b;

        uint32_t grb_color = ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;  // 24bit

        // Iterate through the 24 bits of the color (MSB first)
        for (int j = 23; j >= 0; --j)
        {
            buffer[buffer_index] = ((grb_color >> j) & 0x01) ? COMPARE_CODE_1 : COMPARE_CODE_0;
            buffer_index++;
        }
    }

    assert_param(buffer_index < BUFFER_SIZE);

    while (buffer_index < BUFFER_SIZE)
    {
        buffer[buffer_index] = 0;
        buffer_index++;
    }
}

void sendBuffer() { HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, (uint32_t *)buffer, BUFFER_SIZE); }

void Drivers::WS2812::init()
{
    for (unsigned char &i : buffer)
        i = 0;
    for (auto &color : colors)
        color = {0, 0, 0};
}

void Drivers::WS2812::update()
{
    updateBuffer();
    sendBuffer();
}
