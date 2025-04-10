//
// Created by eeasee on 4/8/25.
//

#include "PositionControl.hpp"

#include "TMAG5170.hpp"

void Tasks::PositionControl::onDataReady()
{
    counterLogs.dataReadyCounter++;
    Drivers::Sensors::TMAG5170::startDMASequentialNormalReadXYZ();
}

void Tasks::PositionControl::updatePosition() {}
