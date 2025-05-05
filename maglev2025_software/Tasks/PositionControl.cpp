//
// Created by eeasee on 4/8/25.
//

#include "PositionControl.hpp"

#include "CoilManager.hpp"
#include "IncrementalPID.hpp"
#include "PID.hpp"
#include "TMAG5170.hpp"

// Utils::IncrementalPID positionPIDAxisX = Utils::IncrementalPID(0.000f, 0.004f, 0.000f, 0.001f);
// Utils::IncrementalPID positionPIDAxisY = Utils::IncrementalPID(0.000f, 0.004f, 0.000f, 0.001f);

Utils::PID positionPIDAxisX = Utils::PID(0.000f, 50.5f, 0.000f, 2005.0f, 0.9f, 1000.0f);
Utils::PID positionPIDAxisY = Utils::PID(0.000f, 50.5f, 0.000f, 2005.0f, 0.9f, 1000.0f);

float offsetCurrentX = 0.0f;  // A and C for X axis;
float offsetCurrentY = 0.0f;  // B and D for Y axis;

float globalOffsetCurrent = 0.0f;

float targetX     = -0.015f;
float targetY     = 0.015f;
float realP       = 50.5f;
float realI       = 0.0000f;
float realD       = 2000.0f;
float lastX       = 0.0f;
float lastY       = 0.0f;
float integralX   = 0.0f;
float integralY   = 0.0f;
float derivativeX = 0.0f;
float derivativeY = 0.0f;
float errorX      = 0.0f;
float errorY      = 0.0f;

void Tasks::PositionControl::updatePosition()
{
    offsetCurrentX = positionPIDAxisX.update(targetX, magMeasurement[0]);
    offsetCurrentY = positionPIDAxisY.update(targetY, magMeasurement[1]);

    //    errorX      = targetX - magMeasurement[0];
    //    derivativeX = (lastX - magMeasurement[0]) * 0.5f + derivativeX * 0.5f;
    //    integralX += errorX;
    //    integralX      = M_CLAMP(integralX, -100.0f, 100.0f);
    //    offsetCurrentX = realP * errorX + realI * integralX + realD * derivativeX;
    //    lastX          = magMeasurement[0];
    //
    //    errorY      = targetY - magMeasurement[1];
    //    derivativeY = (lastY - magMeasurement[1]) * 0.5f + derivativeY * 0.5f;
    //    integralY += errorY;
    //    integralY      = M_CLAMP(integralY, -100.0f, 100.0f);
    //    offsetCurrentY = realP * errorY + realI * integralY + realD * derivativeY;
    //    lastY          = magMeasurement[1];

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
