#include "movement.h"
#include "hardware.h"
#include "maze.h"
#include <cmath>

#define UNIT UNIT_SQUARE
const byte MAX_SPEED = 255;

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

float current_angle;
float current_x;
float current_y;
Direction current_dir;

float target_angle[] =
//   N    NE      E        SE      S       SW         W        NW
    {0, M_PI/4, M_PI/2, 3*M_PI/4, M_PI, -3*M_PI/4, -M_PI/2, -M_PI/4};

#define SQ UNIT_SQUARE
//   N      E     S    W
int offset_x[] = {0, SQ, 0, -SQ};
int offset_y[] = {-SQ, 0, SQ, 0};

void move_to_pose(int x, int y, float r) {
}

#ifdef ARDUINO
void move_to_start() {
}

void move(Direction d) {
    move_to_pose(
        current_x + offset_x[current_dir][d],
        current_y + offset_y[current_dir][d],
        current_angle + target_angle[current_dir][d]
    );
}

#else
#include "Mouse.h"
Wall moving;
char*dir(Direction);
Wall wall_dir[] = {U, UR, R, DR, D, DL, L, UL};
void move(Direction d) { moving = wall_dir[d]; }
void move_to_start()   { moving = H; }

#endif
