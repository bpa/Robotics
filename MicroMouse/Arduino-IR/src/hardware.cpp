#include "hardware.h"

AF_DCMotor right_motor(4);
AF_DCMotor left_motor(3);
Encoder rightOdometer(2, 3);
Encoder leftOdometer(1, 0);
Wheel right(&right_motor, &rightOdometer);
Wheel left(&left_motor, &leftOdometer);

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
            Serial.print(_forward);
            Serial.print(" ");
            Serial.print(d);
            Serial.print(" ");
            Serial.print(target);
            Serial.println();
            return true;
        }
    }
    return false;
}

void Wheel::forward(int target, int speed) {
    long c = encoder->read();
    this->target = target + encoder->read();
    Serial.print("F ");
    Serial.print(target);
    Serial.print(" < ");
    Serial.print(this->target);
    Serial.print(" = ");
    Serial.print(c);
    Serial.println();
    _forward = true;
    targeting = true;
    near = this->target - NEAR;
    motor->setSpeed(speed);
    motor->run(FORWARD);
}

void Wheel::backwards(int target, int speed) {
    long c = encoder->read();
    this->target = encoder->read() - target;
    Serial.print("F ");
    Serial.print(target);
    Serial.print(" > ");
    Serial.print(this->target);
    Serial.print(" = ");
    Serial.print(c);
    Serial.println();
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
