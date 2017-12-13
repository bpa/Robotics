#ifndef MM_HARDWARE_H
#define MM_HARDWARE_H

#include "wheel.h"

#define WHEEL_RADIUS 20
#define WHEEL_SEPARATION 75 
#define GEAR_RATIO 150
#define COUNTS_PER_ROTATION 12
#define PEEK_DISTANCE 40

#define LEFT_MOTOR  3
#define RIGHT_MOTOR 4

#define LEFT_ENCODER_A 1
#define LEFT_ENCODER_B 0

#define RIGHT_ENCODER_A 2
#define RIGHT_ENCODER_B 3

#define FRONT_SENSOR 1
#define LEFT_SENSOR  2
#define RIGHT_SENSOR 3

extern AF_DCMotor right_motor;
extern AF_DCMotor left_motor;

extern Encoder leftOdometer;
extern Encoder rightOdometer;

extern Wheel right;
extern Wheel left;

#endif
