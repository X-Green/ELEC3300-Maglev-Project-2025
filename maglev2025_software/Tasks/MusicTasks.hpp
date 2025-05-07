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
                               {330, 400},
                               {294, 600},
                               {294, 200},
                            {0,0}};

inline MelodyNote melody2[] = {
                                {440,  350},  // a4
                                {440,  350},  // a4
                                {440,  350},  // a4
                                {349,  350},  // f4
                                {523,  450},  // c5
                                {440,  120},  // a4
                                {349,  350},  // f4
                                {523,  450},  // c5
                                {440,  125},  // a4
                                {659,  700},  // e5
                                {659,  350},  // e5
                                {659,  350},  // e5
                                {698,  350},  // f5
                                {523,  225},  // c5
                                {440,  125},  // g5
                                {349,  350},  // f4
                                {523,  225},  // c5
                                {440,  125},  // a4
                                {0,0}
                            };

void startMelody(MelodyNote *notes, size_t size);

void stopMelody();

void update1KHz();

}  // namespace Tasks::Music