#ifndef MM_HARDWARE_H
#define MM_HARDWARE_H

#if defined(ARDUINO)
  #include <AFMotor.h>
  #include <Encoder.h>
#else
  #include "mocks.h"
#endif
#include <math.h>

#define NEAR 5
#define WHEEL_RADIUS 20
#define WHEEL_SEPARATION 75 
#define GEAR_RATIO 150
#define COUNTS_PER_ROTATION 12

#define TICKS_PER_ROTATION (GEAR_RATIO * COUNTS_PER_ROTATION)
#define CIRCUMFERENCE (2 * WHEEL_RADIUS * M_PI)
#define TICK_DISTANCE CIRCUMFERENCE / TICKS_PER_ROTATION
#define TICKS_PER_MM TICKS_PER_ROTATION / CIRCUMFERENCE

class Wheel {
public:
    Wheel(AF_DCMotor *motor, Encoder *encoder);
    bool near_target(long current);
    void forward(int target, int speed);
    void backwards(int target, int speed);
    void reset(long current);
    void stop();

private:
    int near;
    int target;
    int speed;
    bool _forward;
    bool targeting;
    AF_DCMotor *motor;
    Encoder *encoder;
};
    
extern AF_DCMotor right_motor;
extern AF_DCMotor left_motor;
extern Encoder rightOdometer;
extern Encoder leftOdometer;
extern Wheel left;
extern Wheel right;

#define FRONT_SENSOR 1
#define LEFT_SENSOR  2
#define RIGHT_SENSOR 3

#endif
