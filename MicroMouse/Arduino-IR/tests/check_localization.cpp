#include <check.h>
#include "Mouse.h"
#include "hardware.h"
#include "movement.h"

void read_odometry();

typedef struct {
    float initial_r;
    long left;
    long right;
    Pose exp;
    char name[16];
} MoveTest;

#define ck_assert_float(X, Y, T, M) \
do { \
float _ck_x = (X); \
float _ck_y = (Y); \
float _ck_t = (T); \
ck_assert_msg(fabsl(_ck_y - _ck_x) < _ck_t, \
"Assertion %s '%s' failed: %s == %.*g, %s == %.*g, %s == %.*g", \
M, #Y" - "#X" < "#T, \
#X, (int)CK_FLOATING_DIG, _ck_x, \
#Y, (int)CK_FLOATING_DIG, _ck_y, \
#T, (int)CK_FLOATING_DIG, _ck_t); \
} while (0)

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

static const int move_test_count = 6;
static const MoveTest move_test[move_test_count] = {
    {    0,   1000, 1000, 0, -CIRCUMFERENCE,     0,   "North"},
    { M_PI,   1000, 1000, 0,  CIRCUMFERENCE,  M_PI,   "South"},
    { M_PI/2, 1000, 1000,  CIRCUMFERENCE, 0,  M_PI/2, "East" },
    {-M_PI/2, 1000, 1000, -CIRCUMFERENCE, 0, -M_PI/2, "West" },
    {    0,     60,  120, -50, -86.60254,    -M_PI/6, "NW"   },
    {    0,    120,   60,  50, -86.60254,     M_PI/6, "NE"   }
};

START_TEST(test_move_table) {
    MoveTest mt = move_test[_i];
    Pose exp = mt.exp;
    leftOdometer.write(mt.left);
    rightOdometer.write(mt.right);
    pose.x = pose.y = 0;
    pose.r = mt.initial_r;
    read_odometry();
    ck_assert_float(exp.x, pose.x, .0001, mt.name);
    ck_assert_float(exp.y, pose.y, .0001, mt.name);
    ck_assert_float(exp.r, pose.r, .0001, mt.name);
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

