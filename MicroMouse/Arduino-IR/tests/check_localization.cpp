#include <check.h>
#include "Mouse.h"
#include "hardware.h"
#include "movement.h"

void read_odometry();

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

START_TEST(test_north) {
    leftOdometer.write(1000);
    rightOdometer.write(1000);
    pose.r = 0;
    read_odometry();
    ck_assert_float_eq_tol(pose.x, 0, 0.00001);
    ck_assert_float_eq_tol(pose.y, -2 * M_PI * WHEEL_RADIUS, 0.00001);
    ck_assert_float_eq_tol(pose.r, 0, 0.00001);
}
END_TEST

START_TEST(test_east) {
    leftOdometer.write(1000);
    rightOdometer.write(1000);
    pose.r = M_PI/2;
    read_odometry();
    ck_assert_float_eq_tol(pose.x, 2 * M_PI * WHEEL_RADIUS, 0.00001);
    ck_assert_float_eq_tol(pose.y, 0, 0.00001);
    ck_assert_float_eq_tol(pose.r, M_PI/2, 0.00001);
}
END_TEST

START_TEST(test_south) {
    leftOdometer.write(1000);
    rightOdometer.write(1000);
    pose.r = M_PI;
    read_odometry();
    ck_assert_float_eq_tol(pose.x, 0, 0.00001);
    ck_assert_float_eq_tol(pose.y, 2 * M_PI * WHEEL_RADIUS, 0.00001);
    ck_assert_float_eq_tol(pose.r, M_PI, 0.00001);
}
END_TEST

START_TEST(test_west) {
    leftOdometer.write(1000);
    rightOdometer.write(1000);
    pose.r = -M_PI/2;
    read_odometry();
    ck_assert_float_eq_tol(pose.x, -2 * M_PI * WHEEL_RADIUS, 0.00001);
    ck_assert_float_eq_tol(pose.y, 0, 0.00001);
    ck_assert_float_eq_tol(pose.r, -M_PI/2, 0.00001);
}
END_TEST

Suite *localization_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Localization");
    tc_core = tcase_create("Core");
    tcase_add_unchecked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_harnass_overrides);
    tcase_add_test(tc_core, test_north);
    tcase_add_test(tc_core, test_east);
    tcase_add_test(tc_core, test_south);
    tcase_add_test(tc_core, test_west);
    suite_add_tcase(s, tc_core);

    return s;
}

