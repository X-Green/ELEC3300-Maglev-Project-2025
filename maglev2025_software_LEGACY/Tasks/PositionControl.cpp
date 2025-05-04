//
// Created by eeasee on 4/8/25.
//

#include "PositionControl.hpp"

#include "CoilManager.hpp"
#include "IncrementalPID.hpp"
#include "MathUtil.hpp"
#include "TMAG5170.hpp"

Utils::IncrementalPID positionPIDAxisX = Utils::IncrementalPID(0.000f, 2.0f, 0.000f, 0.001f);
Utils::IncrementalPID positionPIDAxisY = Utils::IncrementalPID(0.000f, 2.0f, 0.000f, 0.001f);

float offsetCurrentX = 0.0f;  // A and C for X axis;
float offsetCurrentY = 0.0f;  // B and D for Y axis;

float globalOffsetCurrent = 0.0f;

float realP = 10.5f;
float realI = 0.0000f;
float realD = 215.0f;
float lastX = 0.0f;
float lastY = 0.0f;
float integralX = 0.0f;
float integralY = 0.0f;
float derivativeX = 0.0f;
float derivativeY = 0.0f;
float errorX = 0.0f;
float errorY = 0.0f;

void Tasks::PositionControl::updatePosition()
{
//    // todo: del auto update
//    positionPIDAxisY.kD              = positionPIDAxisX.kD;
//    positionPIDAxisY.kI              = positionPIDAxisX.kI;
//    positionPIDAxisY.kPOnTarget      = positionPIDAxisX.kPOnTarget;
//    positionPIDAxisY.kPOnMeasurement = positionPIDAxisX.kPOnMeasurement;
//    // ===================================================================



    errorX = 0 - magMeasurement[0];
    derivativeX = (lastX - magMeasurement[0]) * 0.9f + derivativeX * 0.1f;
    integralX += errorX;
    integralX = M_CLAMP(integralX, -100.0f, 100.0f);
    offsetCurrentX = realP * errorX + realI * integralX + realD * derivativeX;
    lastX = magMeasurement[0];

    errorY = 0 - magMeasurement[1];
    derivativeY = (lastY - magMeasurement[1]) * 0.9f + derivativeY * 0.1f;
    integralY += errorY;
    integralY = M_CLAMP(integralY, -100.0f, 100.0f);
    offsetCurrentY = realP * errorY + realI * integralY + realD * derivativeY;
    lastY = magMeasurement[1];

//    positionPIDAxisX.update(0, magMeasurement[0]);
//    positionPIDAxisY.update(0, magMeasurement[1]);
//    offsetCurrentX += positionPIDAxisX.getDeltaOutput();
//    offsetCurrentY += positionPIDAxisY.getDeltaOutput();

//    offsetCurrentX = M_CLAMP(offsetCurrentX, -1.0f, 1.0f);
//    offsetCurrentY = M_CLAMP(offsetCurrentY, -1.0f, 1.0f);
    // Which of A and C is x+?
    Tasks::CoilManager::coilTargetCurrent[0] = globalOffsetCurrent + offsetCurrentX;
    Tasks::CoilManager::coilTargetCurrent[2] = globalOffsetCurrent - offsetCurrentX;
    //    // To test
    Tasks::CoilManager::coilTargetCurrent[1] = globalOffsetCurrent + offsetCurrentY;
    Tasks::CoilManager::coilTargetCurrent[3] = globalOffsetCurrent - offsetCurrentY;
}

void Tasks::PositionControl::onDataReady()
{
    counterLogs.dataReadyCounter++;
    Drivers::Sensors::TMAG5170::startDMASequentialNormalReadXYZ();
}
