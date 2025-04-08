//
// Created by eeasee on 4/8/25.
//

#pragma once

namespace Tasks::PositionControl
{
void initPositionControl();

inline float measurement[3] = {0, 0, 0};

/**
 * Unblockingly get data from MAG sensor
 */
void onDataReady();

/**
 *
 */
void updatePosition();

}  // namespace Tasks::PositionControl