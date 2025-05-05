//
// Created by eeasee on 5/3/25.
//

#include "CommandInput.hpp"

#include "tim.h"
#include "usart.h"

uint8_t uartRxBuffer[50] = {0};

/**
 * @brief Encoder Timer
 * Use combine mode of TIM8 CH1 CH2
 *
 */
void Tasks::CommandInput::initCommandInput()
{
    HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uartRxBuffer, 50);
}

/**
 * Start Next DMA Rx in this callback
 */
void Tasks::CommandInput::onU1RXCallback()
{
    Tasks::CommandInput::onVoiceCommandReceived(uartRxBuffer[6]);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uartRxBuffer, 50);
}

void Tasks::CommandInput::onVoiceCommandReceived(uint8_t command) {}