#include "wheel.h"

Wheel::Wheel(AF_DCMotor *motor, Encoder *encoder) {
    this->motor = motor;
    this->encoder = encoder;
    near = 0;
    target = 0;
    speed = 0;
    _forward = true;
    targeting = false;
    motor->run(RELEASE);
}

bool Wheel::near_target(long d) {
    if (targeting) {
        if ( _forward ? d > target : d < target) {
            return true;
        }
    }
    return false;
}

void Wheel::forward(int target, int speed) {
    long c = encoder->read();
    this->target = target + encoder->read();
    _forward = true;
    targeting = true;
    near = this->target - NEAR;
    motor->setSpeed(speed);
    motor->run(FORWARD);
}

void Wheel::backwards(int target, int speed) {
    long c = encoder->read();
    this->target = encoder->read() - target;
    _forward = false;
    targeting = true;
    near = this->target + NEAR;
    motor->setSpeed(speed);
    motor->run(BACKWARD);
}

void Wheel::reset(long current) {
    targeting = false;
    //encoder->write(0);
    //target -= current;
}

void Wheel::stop() {
    motor->run(RELEASE);
    targeting = false;
}
