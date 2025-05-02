//
// Created by eeasee on 5/2/25.
//

#include "WS2812Task.hpp"

const uint32_t BUFFER_SIZE  = Tasks::WS2812Task::LED_COUNT * 24 + 100;  // with reset pulse
uint8_t buffer[BUFFER_SIZE] = {};

/**
 * WS2812 Use Timer2 Channel2; Prescaler=71-1; ARR=3-1
 * 0-Code is [1,0,0] -> Pulse=1
 * 1-Code is [1,1,0] -> Pulse=2
 */

const uint8_t COMPARE_CODE_0 = 1;
const uint8_t COMPARE_CODE_1 = 2;

void updateBuffer()
{
    uint32_t buffer_index = 0;
    for (auto &color : Tasks::WS2812Task::colors)
    {
        // Green, Red, Blue order for 2812
        uint8_t red   = color[0];
        uint8_t green = color[1];
        uint8_t blue  = color[2];

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

void sendBuffer() {
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, (uint32_t *)buffer, BUFFER_SIZE);
}

void Tasks::WS2812Task::initWS2812()
{
    for (unsigned char &i : buffer)
        i = 0;
    for (auto &color : colors)
    {
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
    }
}

void Tasks::WS2812Task::updateWS2812()
{
    updateBuffer();
    sendBuffer();
}
