#ifndef HBOT_GRID_H
#define HBOT_GRID_H

#include "hbot_common.h"

// Initialize grid variables
void init_grid_vars(void);

// Move to coordinate in battery grid
void MoveToCoord(int x, int y);

// Deposit battery
void DepositBattery(void);

// Deposit battery at specific coordinates
void DepositBatteryXY(int x, int y);

// Scoop battery
void ScoopBattery(void);

// Scoop battery at specific coordinates
void ScoopBatteryXY(int x, int y);

#endif // HBOT_GRID_H
