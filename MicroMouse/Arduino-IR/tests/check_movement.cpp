#include <check.h>
#include "maze.h"

extern Pose pose;
extern int logical_x;
extern int logical_y;
extern Direction logical_r;
extern PoseStack points;

static void setup() {
    movement_init();
    logical_x = 3;
    logical_y = 3;
    logical_r = N;
    pose.r = 0;
    pose.x = 3*UNIT_SQUARE + UNIT_SQUARE/2;
    pose.y = 4*UNIT_SQUARE - PEEK_DISTANCE;
}

START_TEST(test_N) {
    _move(N);
    ck_assert_uint_eq(1, points.size);
    ck_assert_float_eq_tol(3*UNIT_SQUARE + UNIT_SQUARE/2, points.data[1].x, 0.001);
    ck_assert_float_eq_tol(3*UNIT_SQUARE + PEEK_DISTANCE, points.data[1].y, 0.001);
    ck_assert_float_eq_tol(0, points.data[1].r, 0.001);

    ck_assert_float_eq_tol(3*UNIT_SQUARE + UNIT_SQUARE/2, points.data[0].x, 0.001);
    ck_assert_float_eq_tol(3*UNIT_SQUARE - PEEK_DISTANCE, points.data[0].y, 0.001);
    ck_assert_float_eq_tol(0, points.data[0].r, 0.001);
}
END_TEST

START_TEST(test_E) {
    _move(E);
    ck_assert_uint_eq(1, points.size);
    ck_assert_float_eq_tol(4*UNIT_SQUARE - PEEK_DISTANCE, points.data[1].x, 0.001);
    ck_assert_float_eq_tol(3*UNIT_SQUARE + UNIT_SQUARE/2, points.data[1].y, 0.001);
    ck_assert_float_eq_tol(M_PI/2, points.data[1].r, 0.001);

    ck_assert_float_eq_tol(4*UNIT_SQUARE + PEEK_DISTANCE, points.data[0].x, 0.001);
    ck_assert_float_eq_tol(3*UNIT_SQUARE + UNIT_SQUARE/2, points.data[0].y, 0.001);
    ck_assert_float_eq_tol(M_PI/2, points.data[0].r, 0.001);
}
END_TEST

START_TEST(test_SE) {
    _move(_SE);
    ck_assert_uint_eq(1, points.size);
    ck_assert_float_eq_tol(3*UNIT_SQUARE + UNIT_SQUARE/2, points.data[1].x, 0.001);
    ck_assert_float_eq_tol(4*UNIT_SQUARE + PEEK_DISTANCE, points.data[1].y, 0.001);
    ck_assert_float_eq_tol(0, points.data[1].r, 0.001);

    ck_assert_float_eq_tol(4*UNIT_SQUARE - PEEK_DISTANCE, points.data[0].x, 0.001);
    ck_assert_float_eq_tol(4*UNIT_SQUARE + UNIT_SQUARE/2, points.data[0].y, 0.001);
    ck_assert_float_eq_tol(-M_PI/2, points.data[0].r, 0.001);
}
END_TEST

START_TEST(test_S) {
    _move(S);
    ck_assert_uint_eq(1, points.size);
    ck_assert_float_eq_tol(3*UNIT_SQUARE + UNIT_SQUARE/2, points.data[1].x, 0.001);
    ck_assert_float_eq_tol(4*UNIT_SQUARE + PEEK_DISTANCE, points.data[1].y, 0.001);
    ck_assert_float_eq_tol(0, points.data[1].r, 0.001);

    ck_assert_float_eq_tol(3*UNIT_SQUARE + UNIT_SQUARE/2, points.data[0].x, 0.001);
    ck_assert_float_eq_tol(5*UNIT_SQUARE - PEEK_DISTANCE, points.data[0].y, 0.001);
    ck_assert_float_eq_tol(0, points.data[0].r, 0.001);
}
END_TEST

START_TEST(test_SW) {
    _move(SW);
    ck_assert_uint_eq(1, points.size);
    ck_assert_float_eq_tol(3*UNIT_SQUARE + UNIT_SQUARE/2, points.data[1].x, 0.001);
    ck_assert_float_eq_tol(4*UNIT_SQUARE + PEEK_DISTANCE, points.data[1].y, 0.001);
    ck_assert_float_eq_tol(0, points.data[1].r, 0.001);

    ck_assert_float_eq_tol(3*UNIT_SQUARE + PEEK_DISTANCE, points.data[0].x, 0.001);
    ck_assert_float_eq_tol(4*UNIT_SQUARE + UNIT_SQUARE/2, points.data[0].y, 0.001);
    ck_assert_float_eq_tol(M_PI/2, points.data[0].r, 0.001);
}
END_TEST

START_TEST(test_W) {
    _move(W);
    ck_assert_uint_eq(1, points.size);
    ck_assert_float_eq_tol(3*UNIT_SQUARE + PEEK_DISTANCE, points.data[1].x, 0.001);
    ck_assert_float_eq_tol(3*UNIT_SQUARE + UNIT_SQUARE/2, points.data[1].y, 0.001);
    ck_assert_float_eq_tol(-M_PI/2, points.data[1].r, 0.001);

    ck_assert_float_eq_tol(3*UNIT_SQUARE - PEEK_DISTANCE, points.data[0].x, 0.001);
    ck_assert_float_eq_tol(3*UNIT_SQUARE + UNIT_SQUARE/2, points.data[0].y, 0.001);
    ck_assert_float_eq_tol(-M_PI/2, points.data[0].r, 0.001);
}
END_TEST

Suite *movement_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Movement");
    tc_core = tcase_create("Core");
    tcase_add_unchecked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_N);
    tcase_add_test(tc_core, test_E);
    tcase_add_test(tc_core, test_SE);
    tcase_add_test(tc_core, test_S);
    tcase_add_test(tc_core, test_SW);
    tcase_add_test(tc_core, test_W);
    suite_add_tcase(s, tc_core);

    return s;
}

