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

typedef struct {
    float initial_r;
    Pose exp;
    char name[16];
} MoveTest;

#define PI   M_PI
#define CIR  CIRCUMFERENCE
static const int move_test_count = 20;
static const MoveTest move_test[move_test_count] = {
//      θ₁          ΔX    ΔY     θ₂     Test Name
    {    0,          0, -CIR,     0,   "North"        },
    {    0,          0,  CIR,     0,   "Bk North"     },
    {   PI,          0,  CIR,    PI,   "South"        },
    {   PI,          0, -CIR,    PI,   "Bk South"     },
    {   PI/2,      CIR,    0,    PI/2, "East"         },
    {   PI/2,     -CIR,    0,    PI/2, "Bk East"      },
    {  -PI/2,     -CIR,    0,   -PI/2, "West"         },
    {  -PI/2,      CIR,    0,   -PI/2, "Bk West"      },
    {    0,     13.398,  -50,    PI/6, "NE"           },
    {    0,     13.398,   50,   -PI/6, "Bk NE"        },
    {    0,    -13.398,  -50,   -PI/6, "NW"           },
    {    0,    -13.398,   50,    PI/6, "Bk NW"        },
    { 3*PI/4,    1.303,    1,  5*PI/6, "SE"           },
    {    0,    -13.398,  -50,   -PI/6, "SW"           },
    {    0,    -13.398,  -50,   -PI/6, "Right only"   },
    {    0,    -13.398,  -50,   -PI/6, "Left only"    },
    {    0,    -13.398,  -50,   -PI/6, "Rotate Right" },
    {    0,    -13.398,  -50,   -PI/6, "Rotate Left"  },
};

#include <stdio.h>
START_TEST(test_move_table) {
    MoveTest mt = move_test[_i];
    Pose exp = mt.exp;
    pose.x = pose.y = 0;
    pose.r = mt.initial_r;
    if (exp.x == 0) {
        double r = mt.initial_r ? exp.y : -exp.y;
        leftOdometer.write(r / TICK_DISTANCE);
        rightOdometer.write(r / TICK_DISTANCE);
    }
    else if (exp.y == 0) {
        double r = mt.initial_r > 0 ? exp.x : -exp.x;
        leftOdometer.write(r / TICK_DISTANCE);
        rightOdometer.write(r / TICK_DISTANCE);
    }
    else {
        double theta = exp.r - mt.initial_r;
        double angle = (M_PI - theta) / 2;
        double m = sqrt(mt.exp.x * mt.exp.x + mt.exp.y * mt.exp.y);
        double r = m * sin(angle) / sin(theta);
        leftOdometer.write((r + 50) * theta / TICK_DISTANCE);
        rightOdometer.write((r - 50) * theta / TICK_DISTANCE);
    }
    read_odometry();
    ck_assert_float(exp.x, pose.x, .5, mt.name);
    ck_assert_float(exp.y, pose.y, .5, mt.name);
    ck_assert_float(exp.r, pose.r, .005, mt.name);
}
END_TEST

Suite *localization_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Localization");
    tc_core = tcase_create("Core");
    tcase_add_unchecked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_harnass_overrides);
    tcase_add_loop_test(tc_core, test_move_table, 0, move_test_count);
    suite_add_tcase(s, tc_core);

    return s;
}

