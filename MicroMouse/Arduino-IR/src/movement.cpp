#include "movement.h"
#include "hardware.h"

#ifdef ARDUINO
  #include <Arduino.h>
#else
  #include <cmath>
  #define abs std::abs
#endif

#define SQ UNIT_SQUARE
const byte MAX_SPEED = 255;
const Pose offset[] = {
    {  0, -SQ,       0  }, //N
    { SQ,   0,  3*M_PI/4}, //NE (Really just E)
    { SQ,   0,  3*M_PI/4}, //E
    { SQ,   0,    M_PI  }, //SE
    {  0, -SQ, -3*M_PI/4}, //S
    { SQ,   0,   -M_PI/2}, //SW
    { SQ,   0,   -M_PI/4}, //W
    { SQ,   0,   -M_PI/4}, //NW (W)
};

Direction logical_r;
int logical_x, logical_y;
Pose pose;
PoseStack points;

bool near_target = false;
bool stopped = true;

void movement_init() {
    logical_x = 0;
    logical_y = 0;
    pose.x = UNIT_SQUARE/2;
    pose.y = UNIT_SQUARE/2;
    pose.r = M_PI;
    logical_r = S;
    points.size = -1;
}

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

void move_to_pose(int x, int y, float r) {
    if (abs(r - pose.r) < .01) {
        left_motor.setSpeed(MAX_SPEED);
        left_motor.run(FORWARD);
        right_motor.setSpeed(MAX_SPEED);
        right_motor.run(FORWARD);
    }
    points.data[0].x = x;
    points.data[0].y = y;
    points.data[0].r = r;
}

void _move(Direction d) {
    move_to_pose(
        points.data[0].x + offset[d].x,
        points.data[0].y + offset[d].y,
        offset[d].r
    );
}

#ifdef ARDUINO
    
void move_to_start() {
    move_to_pose(UNIT_SQUARE/2, UNIT_SQUARE/2, M_PI);
}

#else
#include "Mouse.h"
Wall moving;
char*dir(Direction);
Wall wall_dir[] = {U, UR, R, DR, D, DL, L, UL};
void move(Direction d) { moving = wall_dir[d]; }
void move_to_start()   { moving = H; }
#endif
