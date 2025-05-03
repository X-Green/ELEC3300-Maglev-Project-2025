//
// Created by eeasee on 5/3/25.
//

#include "CommandInput.hpp"

/**
 * @brief Encoder Timer
 * Use combine mode of TIM8 CH1 CH2
 *
 */
void initEncoderTimer() {

}

void initVoiceUART() {}

void Tasks::CommandInput::initCommandInput()
{
    initEncoderTimer();
    initVoiceUART();
}