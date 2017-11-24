#include "hardware.h"
#include "wheel.h"

AF_DCMotor right_motor(RIGHT_MOTOR);
AF_DCMotor left_motor(LEFT_MOTOR);

Encoder leftOdometer(LEFT_ENCODER_A, LEFT_ENCODER_B);
Encoder rightOdometer(RIGHT_ENCODER_A, RIGHT_ENCODER_B);

Wheel right(&right_motor, &rightOdometer);
Wheel left(&left_motor, &leftOdometer);
