#include <check.h>
#include "maze.h"

extern Wall visible[];

START_TEST(test_cardinal) {
    ck_assert_int_eq(cardinal(N,N), N);
    ck_assert_int_eq(cardinal(N,S), S);
    ck_assert_int_eq(cardinal(N,E), E);
    ck_assert_int_eq(cardinal(S,NE), SW);
    ck_assert_int_eq(cardinal(W,NW), SW);
}
END_TEST

START_TEST(test_visible) {
    ck_assert_int_eq(visible[cardinal(E, N)], R);
    ck_assert_int_eq(visible[cardinal(S, E)], L);
    ck_assert_int_eq(visible[cardinal(N, S)], D);
    ck_assert_int_eq(visible[cardinal(N, N)], U);
}
END_TEST

Suite *maze_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Maze");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_cardinal);
    tcase_add_test(tc_core, test_visible);
    suite_add_tcase(s, tc_core);

    return s;
}

