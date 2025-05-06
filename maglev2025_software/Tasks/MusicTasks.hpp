#pragma once

#include "main.h"

namespace Tasks::Music
{

struct MelodyNote
{
    uint16_t frequency;
    uint16_t duration;
};

inline MelodyNote melody1[] = {{330, 200},
                               {330, 200},
                               {349, 200},
                               {392, 200},
                               {392, 200},
                               {349, 200},
                               {330, 200},
                               {294, 200},
                               {262, 200},
                               {262, 200},
                               {294, 200},
                               {330, 200},
                               {330, 250},
                               {294, 100},
                               {294, 200}};

void startMelody(MelodyNote *notes, size_t size);

void stopMelody();

void update1KHz();

}  // namespace Tasks::Music