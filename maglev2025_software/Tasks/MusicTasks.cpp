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

void startMelody(MelodyNote *notes, size_t size)
{
    currentPlaying       = notes;
    currentPlayingIndex  = 0;
    currentPlayingLength = size;
}

void stopMelody()
{
    currentPlaying       = nullptr;
    currentPlayingIndex  = 0;
    currentPlayingLength = 0;
}

void update1KHz()
{
    // 使用静态变量记录当前音符剩余播放时间（ms）
    static uint16_t currentNoteCounter = 0;

    // 如果没有正在播放的旋律，直接返回
    if (currentPlaying == nullptr)
    {
        return;
    }

    // 如果当前音符尚未启动，则启动下一个音符
    if (currentNoteCounter == 0)
    {
        if (currentPlayingIndex < currentPlayingLength)
        {
            MelodyNote note = currentPlaying[currentPlayingIndex];
            Drivers::Buzzer::play(note.frequency, note.duration);
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
