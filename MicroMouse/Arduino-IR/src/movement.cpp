#include "movement.h"
#include "hardware.h"

const byte MAX_SPEED = 255;
const int UNIT = 2450;

bool near_target = false;
bool stopped = true;

void movement() {
    if (stopped)
        return;
    long l = leftOdometer.read();
    long r = rightOdometer.read();
    if (left.near_target(l) || right.near_target(r)) {
        near_target = true;
        left.reset(l);
        right.reset(r);
    }
}

void move_right() {
    near_target = false;
    stopped = false;
    stop();
}

void move_left() {
    near_target = false;
    stopped = false;
    stop();
}

void move_forward() {
    near_target = false;
    stopped = false;
    left.forward(UNIT, MAX_SPEED);
    right.forward(UNIT, MAX_SPEED);
}

void move_backward() {
    near_target = false;
    stopped = false;
    left.backwards(UNIT, MAX_SPEED);
    right.backwards(UNIT, MAX_SPEED);
}

void stop() {
    near_target = false;
    stopped = true;
    left.stop();
    right.stop();
}
