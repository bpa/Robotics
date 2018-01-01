#include "hardware.h"
#include "queue.h"
#include "movement.h"

#ifdef ARDUINO
  #include <Arduino.h>
#else
  #include <cmath>
  #define abs std::abs
#endif

#define SQ UNIT_SQUARE
const byte MAX_SPEED = 255;
const Pose offset[] = {
    { SQ/2, SQ-PEEK_DISTANCE,     0  }, //N
    { 0,0,0 }, //NE (invalid)
    { PEEK_DISTANCE,    SQ/2,  M_PI/2}, //E
    { 0,0,0 }, //SE (invalid)
    { SQ/2,    PEEK_DISTANCE,  M_PI  }, //S
    { 0,0,0 }, //SW (invalid)
    { SQ-PEEK_DISTANCE, SQ/2, -M_PI/2}, //W
    { 0,0,0 }, //NW (invalid)
};
const int xy_diff[][2] = {
    //X,  Y
    { 0, -1}, //N
    { 0, -1}, //NE
    { 1,  0}, //E
    { 0,  1}, //SE
    { 0,  1}, //S
    { 0,  1}, //SW
    {-1,  0}, //W
    { 0, -1}, //NW
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

void _move(Direction d) {
    queue_init(points);
    Direction act = cardinal(d, logical_r);
    Pose fwd = offset[act];
    Pose rev = offset[cardinal(act, S)];
    Pose one = {logical_x * SQ + rev.x, logical_y * SQ + rev.y, fwd.r};
    logical_x += xy_diff[act][0];
    logical_y += xy_diff[act][1];
    Pose two = {logical_x * SQ + fwd.x, logical_y * SQ + fwd.y, fwd.r};
    queue_push(points, two);
    queue_push(points, one);
    logical_r = act;
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
