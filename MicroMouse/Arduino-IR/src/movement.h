#ifndef MM_MOVEMENT_H
#define MM_MOVEMENT_H

#include "hardware.h"

#define TICKS_PER_ROTATION (GEAR_RATIO * COUNTS_PER_ROTATION)
#define CIRCUMFERENCE (2 * WHEEL_RADIUS * M_PI)
#define TICK_DISTANCE CIRCUMFERENCE / TICKS_PER_ROTATION
#define TICKS_PER_MM TICKS_PER_ROTATION / CIRCUMFERENCE

extern bool near_target;
extern bool stopped;

extern void movement();
extern void move_right();
extern void move_left();
extern void move_forward();
extern void move_backward();
extern void stop();

#endif
