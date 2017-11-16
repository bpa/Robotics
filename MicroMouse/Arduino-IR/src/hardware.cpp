#include "hardware.h"

AF_DCMotor right_motor(3);
AF_DCMotor left_motor(4);
Encoder rightOdometer(1, 0);
Encoder leftOdometer(2, 3);
Wheel right(&right_motor, &rightOdometer);
Wheel left(&left_motor, &leftOdometer);

Wheel::Wheel(AF_DCMotor *motor, Encoder *encoder) {
    this->motor = motor;
    this->encoder = encoder;
    motor->run(RELEASE);
}

bool Wheel::near_target(long d) {
    if (targeting) {
        return _forward ? d > target : d < target;
    }
    return false;
}

void Wheel::forward(int target, int speed) {
    this->target += target;
    _forward = true;
    targeting = true;
    near = target - NEAR;
    motor->setSpeed(speed);
    motor->run(FORWARD);
}

void Wheel::backwards(int target, int speed) {
    this->target -= target;
    _forward = false;
    targeting = true;
    near = target + NEAR;
    motor->setSpeed(speed);
    motor->run(BACKWARD);
}

void Wheel::reset(long current) {
    targeting = false;
    encoder->write(current - target);
    target -= current;
}

void Wheel::stop() {
    motor->run(RELEASE);
    targeting = false;
}
