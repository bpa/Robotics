#ifndef MM_HARDWARE_H
#define MM_HARDWARE_H

#include "wheel.h"

#ifdef ARDUINO
  #define WHEEL_RADIUS 20
  #define WHEEL_SEPARATION 75
  #define GEAR_RATIO 150
  #define COUNTS_PER_ROTATION 12
  #define PEEK_DISTANCE 40
#else
  //Need some test numbers that don't change
  #define WHEEL_RADIUS 25
  #define WHEEL_SEPARATION 100
  #define GEAR_RATIO 100
  #define COUNTS_PER_ROTATION 10
  #define PEEK_DISTANCE 50
#endif

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
