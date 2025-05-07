#pragma once

#include "main.h"

namespace Tasks::Music
{

struct MelodyNote
{
    uint16_t frequency;
    uint16_t duration;
};

inline MelodyNote melody1[] = {{330, 400},
                               {330, 400},
                               {349, 400},
                               {392, 400},
                               {392, 400},
                               {349, 400},
                               {330, 400},
                               {294, 400},
                               {262, 400},
                               {262, 400},
                               {294, 400},
                               {330, 400},
                               {330, 600},
                               {294, 200},
                               {294, 400}};



void startMelody(MelodyNote *notes, size_t size);

void stopMelody();

void update1KHz();

}  // namespace Tasks::Music