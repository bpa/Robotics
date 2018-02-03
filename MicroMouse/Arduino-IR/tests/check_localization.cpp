#include <check.h>
#include "Mouse.h"
#include "hardware.h"
#include "movement.h"

void read_odometry();

#define ck_assert_float(X, Y, T, M) \
do { \
double _ck_x = (X); \
double _ck_y = (Y); \
double _ck_t = (T); \
ck_assert_msg(fabsl(_ck_y - _ck_x) < _ck_t, \
"Assertion %s '%s' failed: %s == %.*g, %s == %.*g, %s == %.*g", \
M, #Y" - "#X" < "#T, \
#X, (int)CK_FLOATING_DIG, _ck_x, \
#Y, (int)CK_FLOATING_DIG, _ck_y, \
#T, (int)CK_FLOATING_DIG, _ck_t); \
} while (0)

#define V(a, r) ((a) * (r + WHEEL_SEPARATION / 2)), \
                ((a) * (r - WHEEL_SEPARATION / 2))
#define rad2deg(r) (r * 180 / M_PI)

static void setup() {
    movement_init();
    analogWrite(FRONT_SENSOR, 35);
    analogWrite(RIGHT_SENSOR, 35);
    analogWrite(LEFT_SENSOR, 35);
    pose.x = 0;
    pose.y = 0;
    pose.r = 0;
}

START_TEST(test_harnass_overrides) {
    ck_assert_uint_eq(TICKS_PER_ROTATION, 1000);
    ck_assert_uint_eq(WHEEL_RADIUS, 25);
    ck_assert_float_eq(CIRCUMFERENCE, 50 * M_PI );
    ck_assert_float_eq(TICK_DISTANCE, 50 * M_PI / 1000);
}
END_TEST

enum Side {
    RL = -1,
    RR = 1
};

typedef struct {
    double initial_r;
    double x;
    double y;
    double rotation;
    Side left;
    char name[16];
} MoveTest;

#define PI   M_PI
#define CIR  CIRCUMFERENCE
static const int move_test_count = 20;
static const MoveTest move_test[move_test_count] = {
//                    Non 0 is rotation
//      θ₁      ΔX    ΔY     θ₂ AXIS Test Name
    {    0,      0, -CIR,     0, RR,  "North"        },
    {    0,      0,  CIR,     0, RR,  "Bk North"     },
    {   PI,      0,  CIR,     0, RR,  "South"        },
    {   PI,      0, -CIR,     0, RR,  "Bk South"     },
    {   PI/2,  CIR,    0,     0, RR,  "East"         },
    {   PI/2, -CIR,    0,     0, RR,  "Bk East"      },
    {  -PI/2, -CIR,    0,     0, RR,  "West"         },
    {  -PI/2,  CIR,    0,     0, RR,  "Bk West"      },
    {    0,     25,  -70,     0, RR,  "NE"           },
    {    0,     25,   70,     0, RR,  "Bk NE"        },
    {    0,    -25,  -70,     0, RL,  "NW"           },
    {    0,    -25,   70,     0, RL,  "Bk NW"        },
    { 3*PI/4,    2,    1,     0, RL,  "SE"           },
    { 3*PI/4,   -2,   -1,     0, RR,  "Bk SE"        },
    {-5*PI/6,   20,   50,     0, RL,  "SW"           },
    {-5*PI/6,   20,  -50,     0, RL,  "Bk SW"        },
    {    0,    -50,  -50,     0, RR,  "Right only"   },
    {    0,     50,  -50,     0, RL,  "Left only"    },
    {    0,      0,    0,  PI/3, RR,  "Rotate Right" },
    {    0,      0,    0, -PI/3, RL,  "Rotate Left"  },
};

#include <stdio.h>
START_TEST(test_move_table) {
    MoveTest mt = move_test[_i];
    pose.x = pose.y = 0;
    pose.r = mt.initial_r;
    double exp_r = mt.initial_r;
    if (mt.rotation != 0) {
        double theta = mt.rotation - mt.initial_r;
        exp_r = mt.rotation;
        leftOdometer.write((WHEEL_SEPARATION/2) * theta / TICK_DISTANCE);
        rightOdometer.write((-WHEEL_SEPARATION/2) * theta / TICK_DISTANCE);
    }
    else if (mt.x == 0) {
        double r = mt.initial_r ? mt.y : -mt.y;
        leftOdometer.write(r / TICK_DISTANCE);
        rightOdometer.write(r / TICK_DISTANCE);
    }
    else if (mt.y == 0) {
        double r = mt.initial_r > 0 ? mt.x : -mt.x;
        leftOdometer.write(r / TICK_DISTANCE);
        rightOdometer.write(r / TICK_DISTANCE);
    }
    else {
        double m = mt.left * sqrt(mt.x * mt.x + mt.y * mt.y);
        double angle = -atan(mt.y / mt.x);
        double theta = angle > 0 ? M_PI - 2 * angle : -M_PI - 2 * angle;
        exp_r = pose.r + theta;
        double r = m * sin(angle) / sin(theta);
        leftOdometer.write((r + 50) * theta / TICK_DISTANCE);
        rightOdometer.write((r - 50) * theta / TICK_DISTANCE);
        printf("--- %s\n", mt.name);
        printf("X: %.1f Y: %.1f ω: %.0f° ", mt.x, mt.y, rad2deg(exp_r));
        printf("r: %.3f m: %.3f ", r, m);
        printf("θ: %.0f° α: %.0f° ", rad2deg(theta), rad2deg(angle));
        printf("L: %ld ",  (long)((r + 50) * theta / TICK_DISTANCE));
        printf("R: %ld\n", (long)((r - 50) * theta / TICK_DISTANCE));
    }
    read_odometry();
    ck_assert_float(mt.x, pose.x, .5, mt.name);
    ck_assert_float(mt.y, pose.y, .5, mt.name);
    ck_assert_float(exp_r, pose.r, .005, mt.name);
}
END_TEST

Suite *localization_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Localization");
    tc_core = tcase_create("Core");
    tcase_add_unchecked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_harnass_overrides);
    tcase_add_loop_test(tc_core, test_move_table, 12, move_test_count);
    suite_add_tcase(s, tc_core);

    return s;
}

