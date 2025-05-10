#pragma once
#include "main.h"

#define LED_NUM 14
#define RESET_COUNT 50
#define WS2812_BIT1_WIDTH 0x2
#define WS2812_BIT0_WIDTH 0x1

namespace Drivers::WS2812
{

inline bool isInited = false;

/**
 * @brief The structure that contains the rgb value of a single ws2812 unit
 */
struct RGB
{
    uint8_t green;
    uint8_t red;
    uint8_t blue;

    RGB(uint8_t r, uint8_t g, uint8_t b) : green(g), red(r), blue(b) {}
    RGB(uint8_t r, uint8_t g, uint8_t b, float luminance)
        : green(g * luminance), red(r * luminance), blue(b * luminance)
    {
    }

    void toBitSequence(uint32_t *bitSequence) const;
};

inline RGB RGB_RED(255, 0, 0);
inline RGB RGB_ORANGE(255, 127, 0);
inline RGB RGB_YELLOW(255, 255, 0);
inline RGB RGB_GREEN(0, 255, 0);
inline RGB RGB_CYAN(0, 255, 255);
inline RGB RGB_BLUE(0, 0, 255);
inline RGB RGB_PURPLE(127, 0, 255);

inline RGB RGB_WHITE(255, 255, 255);
inline RGB RGB_BLACK(0, 0, 0);

void setColor(int index, unsigned char r, unsigned char g, unsigned char b);
void setColor(int index, RGB color);
void setColor(int index, RGB color, float brightness);

void blank(int index);

void blankAll();

void init();

}  // namespace Drivers::WS2812
inline uint32_t CCRDMABuff[LED_NUM * 24 + RESET_COUNT] = {};

// #endif