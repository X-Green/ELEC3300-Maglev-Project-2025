//
// Created by eeasee on 2025/5/6.
//

#include "MusicTasks.hpp"

#include "Buzzer.hpp"

namespace Tasks::Music
{

MelodyNote *currentPlaying  = nullptr;
size_t currentPlayingIndex  = 0;
size_t currentPlayingLength = 0;
uint16_t currentNoteCounter = 0;

void startMelody(MelodyNote *notes, size_t size)
{
    currentPlaying       = notes;
    currentPlayingIndex  = 0;
    currentPlayingLength = size;
    currentNoteCounter  = 0;
}

void stopMelody()
{
    currentPlaying       = nullptr;
    currentPlayingIndex  = 0;
    currentPlayingLength = 0;
    currentNoteCounter  = 0;
}

void update1KHz()
{


    if (currentPlaying == nullptr)
    {
        return;
    }

    if (currentNoteCounter == 0)
    {
        if (currentPlayingIndex < currentPlayingLength)
        {
            MelodyNote note = currentPlaying[currentPlayingIndex];
            Drivers::Buzzer::play(note.frequency, note.duration - 25);
            currentNoteCounter = note.duration;
        }
        else
        {
            stopMelody();
            return;
        }
    }

    currentNoteCounter--;
    if (currentNoteCounter == 0)
    {
        currentPlayingIndex++;
    }
}

}  // namespace Tasks::Music
