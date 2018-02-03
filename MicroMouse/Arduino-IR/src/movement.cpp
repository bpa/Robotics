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
long odo_r, odo_l;

void movement_init() {
    logical_x = 0;
    logical_y = 0;
    pose.x = UNIT_SQUARE/2;
    pose.y = UNIT_SQUARE/2;
    pose.r = M_PI;
    logical_r = S;
    points.size = 0;
    points.data[0].x = pose.x;
    points.data[0].y = pose.y;
    points.data[0].r = pose.r;
    odo_l = leftOdometer.read();
    odo_r = rightOdometer.read();
}

void ignore() {}
void (*look_around)() = ignore;

#include <stdio.h>
#define rad2deg(r) (r * 180 / M_PI)
void read_odometry() {
    double m, angle;
    long l = leftOdometer.read();
    long r = rightOdometer.read();
    if (l == r) {
        m = l * TICK_DISTANCE;
        angle = pose.r;
    }
    else {
        double dL = l * TICK_DISTANCE;
        double dR = r * TICK_DISTANCE;
        double a2 = (dL - dR) / 2 / WHEEL_SEPARATION + pose.r;
        double radius = WHEEL_SEPARATION * (dL + dR) / (dL - dR) / 2;
        double theta = (dL - dR) / WHEEL_SEPARATION;
        double theta_abs = abs(theta);
        m = radius * sin(theta_abs) / sin((M_PI - theta_abs)/2);
        angle = pose.r + theta / 2;
    pose.x += m * sin(angle);
    pose.y -= m * cos(angle);
        pose.r += theta;
        printf("X: %.1f Y: %.1f ω: %.0f° ", pose.x, pose.y, rad2deg(pose.r));
        printf("r: %.3f m: %.3f ", radius, m);
        printf("θ: %.0f° α: %.0f° ", rad2deg(theta), rad2deg(angle));
        printf("dL: %3.1f dR: %3.1f\n", dL, dR);
        return;
    }
    pose.x += m * sin(angle);
    pose.y -= m * cos(angle);
}

void update_position() {
    read_odometry();
}

void move_forward(Direction d) {
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

void move_backwards(Direction d) {
    Direction act = cardinal(d, logical_r);
    Direction south = cardinal(S, logical_r);
    logical_x += xy_diff[south][0];
    logical_y += xy_diff[south][1];
    Pose rev = offset[south];
    Pose turn = offset[act];
    Pose one = {logical_x * SQ +  rev.x, logical_y * SQ +  rev.y, pose.r};
    Pose two = {logical_x * SQ + turn.x, logical_y * SQ + turn.y, turn.r};
    queue_push(points, two);
    queue_push(points, one);
    logical_r = act;
}

void _move(Direction d) {
    queue_init(points);
    switch(d) {
        case N:
        case E:
        case W:
            move_forward(d);
            break;
        case S:
            move_backwards(N);
            break;
        case _SE:
            move_backwards(W);
            break;
        case SW:
            move_backwards(E);
            break;
    }
}

#ifdef ARDUINO
    
void move_to_start() {
    move_to_pose(UNIT_SQUARE/2, UNIT_SQUARE/2, M_PI);
}

bool near_target() {
    return (abs(pose.x - points.data[0].x) + abs(pose.y - points.data[0].y)) < 10;
}

#else
#include "Mouse.h"
Wall moving;
char*dir(Direction);
Wall wall_dir[] = {U, UR, R, DR, D, DL, L, UL};
void move(Direction d) { moving = wall_dir[d]; }
void move_to_start()   { moving = H; }
bool near_target() { return true; }
#endif
