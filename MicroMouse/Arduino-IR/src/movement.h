#ifndef MM_MOVEMENT_H
#define MM_MOVEMENT_H

#include "hardware.h"

#define TICKS_PER_ROTATION (GEAR_RATIO * COUNTS_PER_ROTATION)
#define CIRCUMFERENCE (2 * WHEEL_RADIUS * M_PI)
#define TICK_DISTANCE CIRCUMFERENCE / TICKS_PER_ROTATION
#define TICKS_PER_MM TICKS_PER_ROTATION / CIRCUMFERENCE

extern bool near_target;
extern bool stopped;

void movement();
void move_right();
void move_left();
void move_forward();
void move_backward();
void move_back_left();
void move_back_right();
void move_to_pose(int x, int y, float r);
void stop();

#endif
