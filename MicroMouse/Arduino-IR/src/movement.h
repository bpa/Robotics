#ifndef MM_MOVEMENT_H
#define MM_MOVEMENT_H

#include "hardware.h"
#include "maze.h"

#define TICKS_PER_ROTATION (GEAR_RATIO * COUNTS_PER_ROTATION)
#define CIRCUMFERENCE (2 * WHEEL_RADIUS * M_PI)
#define TICK_DISTANCE CIRCUMFERENCE / TICKS_PER_ROTATION
#define TICKS_PER_MM TICKS_PER_ROTATION / CIRCUMFERENCE

typedef struct {
    float x;
    float y;
    float r;
} Pose;

typedef struct {
    int size;
    Pose data[2];
} PoseStack;

extern Direction current_dir;
extern bool near_target;
extern bool stopped;
extern const Wall visible[];
extern Pose current_pose;

void movement_init();
void movement();
#ifdef ARDUINO
  //Need a test seam
  #define move(d) _move(d)
#else
void move(Direction);
#endif 
void _move(Direction);
void move_to_start();
void move_to_pose(int x, int y, float r);

#endif
