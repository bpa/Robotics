#include <check.h>
#include <cmath>
#include "Mouse.h"
#include "hardware.h"
#include "movement.h"

void read_odometry(bool);
const char RED[]   = "\e[30;41m";
const char GREEN[] = "\e[37;42m";

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

// 12 starting directions * 5 direction changes * 3 distances
static const int move_test_count = 12 * 5 * 3;

double diff(double act, double exp) {
    return fabs(act) - fabs(exp);
}

double rdiff(double act, double exp) {
    if (fabs(act) > M_PI)
        return fabs(act) - fabs(exp);
    double r_actual = act < -0.1 ? act + 2 * M_PI : act;
    double r_wanted = exp < -0.1 ? exp + 2 * M_PI : exp;
    return fabs(r_actual) - fabs(r_wanted);
}

#include <stdio.h>
START_TEST(test_move_table) {
    double exp_x, exp_y;
    int dist = _i % 5;
    int init = _i / 5;
    int dir = init % 5;
    init /= 5;
    double initial_r = M_PI * init / 6;

    pose.x = pose.y = 0;
    pose.r = initial_r;
    double theta = M_PI / 3 - dir * M_PI / 6;
    double m = (dist - 1) * 50;
    double r = sin(theta / 2) * m / sin(theta) * copysign(1, theta);
    double R = (r - WHEEL_SEPARATION / 2) * theta;
    double L = (r + WHEEL_SEPARATION / 2) * theta;

    double exp_r = initial_r + theta / 2;
    if (fabs(exp_r) > M_PI) {
        exp_r -= exp_r < 0 ? -2*M_PI : 2*M_PI;
    }

    exp_x = sin(exp_r) * m;
    exp_y = cos(exp_r) * m;

    if (theta == 0) {
        R = L = m;
    }

    leftOdometer.write(L / TICK_DISTANCE);
    rightOdometer.write(R / TICK_DISTANCE);

    read_odometry(false);

    if (diff(pose.x, exp_x) > .5 || diff(pose.y, exp_y) > .5 || rdiff(pose.r, exp_r) > .5) {
        printf("→α: %3.0f° ", rad2deg(initial_r));
        printf("θ: %3.0f° r: %6.2f m: %3.0f ", rad2deg(theta), r, m);
        printf("(%7.2f, %7.2f, %4.0f°) ", exp_x, exp_y, rad2deg(exp_r));
        printf("L: %s%5.0f\e[0m", L > 0 ? GREEN : RED, L);
        printf("R: %s%5.0f\e[0m\n", R > 0 ? GREEN : RED, R);
        pose.x = pose.y = 0;
        pose.r = initial_r;
        leftOdometer.write(L / TICK_DISTANCE);
        rightOdometer.write(R / TICK_DISTANCE);

        printf("Xα: %3.0f° ", rad2deg(initial_r));
        read_odometry(true);

        ck_assert_float(exp_x, pose.x, .5, "");
        ck_assert_float(exp_y, pose.y, .5, "");
        ck_assert_float(exp_r, pose.r, .5, "");
    }
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

