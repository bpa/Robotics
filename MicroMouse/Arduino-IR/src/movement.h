#ifndef MM_MOVEMENT_H
#define MM_MOVEMENT_H

#include "hardware.h"
#include "maze.h"

#define TICKS_PER_ROTATION (GEAR_RATIO * COUNTS_PER_ROTATION)
#define CIRCUMFERENCE (2 * WHEEL_RADIUS * M_PI)
#define TICK_DISTANCE CIRCUMFERENCE / TICKS_PER_ROTATION
#define TICKS_PER_MM TICKS_PER_ROTATION / CIRCUMFERENCE

extern Direction current_dir;
extern float current_angle;
extern float current_x;
extern float current_y;
extern bool near_target;
extern bool stopped;

void movement();
void move(Direction);
void move_to_start();

void move_to_pose(int x, int y, float r);

#endif
