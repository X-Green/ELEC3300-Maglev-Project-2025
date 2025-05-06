//
// Created by eeasee on 5/6/25.
//

#include "LEDTasks.hpp"

#include "MathUtil.hpp"
#include "WS2812.hpp"

uint32_t kHzCount = 0;

Drivers::WS2812::RGB rainbowSequence[7] = {Drivers::WS2812::RGB_RED,
                                           Drivers::WS2812::RGB_ORANGE,
                                           Drivers::WS2812::RGB_YELLOW,
                                           Drivers::WS2812::RGB_GREEN,
                                           Drivers::WS2812::RGB_CYAN,
                                           Drivers::WS2812::RGB_BLUE,
                                           Drivers::WS2812::RGB_PURPLE};

void Tasks::LEDTasks::kHzTrigger()
{
    kHzCount++;
    if (kHzCount > 10000)
    {
        kHzCount = 0;
        // 10 Sec Loop
    }

    if (kHzCount % 10 != 0)
    {
        return;
        // 100Hz Update
    }

    static int rainbowCycleColorIdx = 0;
    static uint32_t lastBreathCycle = 0;

    switch (ledPattern)
    {
    case LED_PATTERN_OFF:
    {
        Drivers::WS2812::blankAll();
        break;
    }
    case LED_PATTERN_WARNING:
    {
        // 1Hz change between 100 and 0 gamma
        float brightness = (float)(kHzCount % 2000) / 1000.0f - 1.0f;
        brightness       = M_ABS(brightness);
        for (int i = 0; i < LED_NUM; ++i)
            Drivers::WS2812::setColor(i, (uint8_t)(brightness * 255), 0, 0);
        break;
    }
    case LED_PATTERN_RAINBOW:
    {
        if (kHzCount % 100 != 0)
            return;
        Drivers::WS2812::setColor(0, Drivers::WS2812::RGB_RED);
        Drivers::WS2812::setColor(1, Drivers::WS2812::RGB_ORANGE);
        Drivers::WS2812::setColor(2, Drivers::WS2812::RGB_YELLOW);
        Drivers::WS2812::setColor(3, Drivers::WS2812::RGB_GREEN);
        Drivers::WS2812::setColor(4, Drivers::WS2812::RGB_CYAN);
        Drivers::WS2812::setColor(5, Drivers::WS2812::RGB_BLUE);
        Drivers::WS2812::setColor(6, Drivers::WS2812::RGB_PURPLE);
        Drivers::WS2812::setColor(7, Drivers::WS2812::RGB_RED);
        Drivers::WS2812::setColor(8, Drivers::WS2812::RGB_ORANGE);
        Drivers::WS2812::setColor(9, Drivers::WS2812::RGB_YELLOW);
        Drivers::WS2812::setColor(10, Drivers::WS2812::RGB_GREEN);
        Drivers::WS2812::setColor(11, Drivers::WS2812::RGB_CYAN);
        Drivers::WS2812::setColor(12, Drivers::WS2812::RGB_BLUE);
        Drivers::WS2812::setColor(13, Drivers::WS2812::RGB_PURPLE);
        break;
    }
    case LED_PATTERN_RAINBOW_BREATHING:
    {
        // Breathing effect: brightness varies smoothly between 0 and 1
        // Period: 2 seconds (2000 ms), so use kHzCount % 2000
        float phase      = (float)(kHzCount % 2000) / 2000.0f;               // 0 to 1
        float brightness = 0.5f * (1.0f + sinf(phase * 2.0f * 3.1415926f));  // 0 to 1

        for (int i = 0; i < LED_NUM; ++i)
        {
            // Repeat rainbow sequence across LEDs
            int colorIdx = i % 7;
            Drivers::WS2812::setColor(i, rainbowSequence[colorIdx], brightness);
        }
        break;
    }
    case LED_PATTERN_RAINBOW_CYCLE:
    {
        // Smoothly cycle the rainbow pattern along the LEDs
        // The offset advances smoothly over time
        float speed = 0.02f; // Adjust for desired speed (LEDs per update)
        static float rainbowOffset = 0.0f;
        rainbowOffset += speed;
        if (rainbowOffset >= 7.0f) rainbowOffset -= 7.0f;

        for (int i = 0; i < LED_NUM; ++i)
        {
            // Calculate fractional color index for smooth transition
            float idx = fmodf(i + rainbowOffset, 7.0f);
            int idx0 = (int)idx;
            int idx1 = (idx0 + 1) % 7;
            float frac = idx - idx0;

            // Linear interpolation between two colors
            Drivers::WS2812::RGB c0 = rainbowSequence[idx0];
            Drivers::WS2812::RGB c1 = rainbowSequence[idx1];
            uint8_t r = (uint8_t)((1.0f - frac) * c0.red   + frac * c1.red);
            uint8_t g = (uint8_t)((1.0f - frac) * c0.green + frac * c1.green);
            uint8_t b = (uint8_t)((1.0f - frac) * c0.blue  + frac * c1.blue);

            Drivers::WS2812::setColor(i, r, g, b);
        }
        break;
    }
    case LED_PATTERN_RAINBOW_BREATHING_CYCLE:
    {
        // Breathing effect: brightness varies smoothly between 0 and 1
        float phase      = (float)(kHzCount % 2000) / 1000.0f;  // 0 to 2
        float brightness = M_ABS(phase - 1);

        // On each new breathing cycle, advance the color index
        if (phase == 1.0f)
        {
            rainbowCycleColorIdx = (rainbowCycleColorIdx + 1) % 7;
            lastBreathCycle      = (kHzCount / 2000);
        }

        for (int i = 0; i < LED_NUM; ++i)
        {
            Drivers::WS2812::setColor(i, rainbowSequence[rainbowCycleColorIdx], brightness);
        }
        break;
    }
    default:
        Drivers::WS2812::blankAll();
    }
}

