#include "movement.h"
#include "hardware.h"

const byte MAX_SPEED = 255;
const int UNIT = 2450;

bool near_target = false;
bool stopped = true;

void ignore() {}
void center() {
    if (analogRead(FRONT_SENSOR) > 175) {
        near_target = true;
        return;
    }

    int l = analogRead(LEFT_SENSOR);
    int r = analogRead(RIGHT_SENSOR);
    if (l > 200) {
        left_motor.setSpeed(MAX_SPEED);
        right_motor.setSpeed(MAX_SPEED - 75);
        return;
    }

    if (r > 200) {
        left_motor.setSpeed(MAX_SPEED - 75);
        right_motor.setSpeed(MAX_SPEED);
        return;
    }

    left_motor.setSpeed(MAX_SPEED);
    right_motor.setSpeed(MAX_SPEED);
}

void (*look_around)() = ignore;

void movement() {
    if (stopped)
        return;
    long l = leftOdometer.read();
    long r = rightOdometer.read();
    if (left.near_target(l) || right.near_target(r)) {
        near_target = true;
        left.reset(l);
        right.reset(r);
        Serial.println("close");
    }
    else {
        look_around();
    }
}

#ifdef ARDUINO
void move_right() {
    near_target = false;
    stopped = false;
    left.forward(1400, MAX_SPEED);
    right.backwards(790, 50);
    look_around = ignore;
}

void move_left() {
    near_target = false;
    stopped = false;
    Serial.println("in left");
    left.backwards(700, 50);
    right.forward(1400, MAX_SPEED);
    look_around = ignore;
}

void move_forward() {
    near_target = false;
    stopped = false;
    left.forward(UNIT, MAX_SPEED);
    right.forward(UNIT, MAX_SPEED);
    look_around = center;
}

void move_backward() {
    near_target = false;
    stopped = false;
    left.backwards(UNIT, MAX_SPEED);
    right.backwards(UNIT, MAX_SPEED);
    look_around = center;
}

void stop() {
    near_target = false;
    stopped = true;
    left.stop();
    right.stop();
    look_around = ignore;
}
#else
#include "Mouse.h"
Wall moving;
void move_right()      { moving = R;  }
void move_left()       { moving = L;  }
void move_forward()    { moving = U;  }
void move_backward()   { moving = D;  }
void move_back_left()  { moving = DL; }
void move_back_right() { moving = DR; }
void stop() {}
#endif
