//
// Created by eeasee on 4/8/25.
//

#include "PositionControl.hpp"

#include "CoilManager.hpp"
#include "IncrementalPID.hpp"
#include "TMAG5170.hpp"

Utils::IncrementalPID positionPIDAxisX = Utils::IncrementalPID(0.000f, 0.004f, 0.000f, 0.001f);
Utils::IncrementalPID positionPIDAxisY = Utils::IncrementalPID(0.000f, 0.004f, 0.000f, 0.001f);

float offsetCurrentX = 0.0f;  // A and C for X axis;
float offsetCurrentY = 0.0f;  // B and D for Y axis;

float globalOffsetCurrent = 1.0f;

void Tasks::PositionControl::updatePosition()
{
    positionPIDAxisX.update(0, magMeasurement[0]);
    positionPIDAxisY.update(0, magMeasurement[1]);
    offsetCurrentX += positionPIDAxisX.getDeltaOutput();
    offsetCurrentY += positionPIDAxisY.getDeltaOutput();
    // Which of A and C is x+?
    Tasks::CoilManager::coilTargetCurrent[0] = globalOffsetCurrent + offsetCurrentX;
    Tasks::CoilManager::coilTargetCurrent[2] = globalOffsetCurrent - offsetCurrentX;
    // To test
    Tasks::CoilManager::coilTargetCurrent[1] = globalOffsetCurrent + offsetCurrentY;
    Tasks::CoilManager::coilTargetCurrent[3] = globalOffsetCurrent - offsetCurrentY;
}

void Tasks::PositionControl::onDataReady()
{
    counterLogs.dataReadyCounter++;
    Drivers::Sensors::TMAG5170::startDMASequentialNormalReadXYZ();
}
