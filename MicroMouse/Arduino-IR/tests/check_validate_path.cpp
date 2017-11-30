#include <check.h>
#include "maze.h"

static void setup() {
    analogWrite(FRONT_SENSOR, 35);
    INIT();
    state = VALIDATE_SHORTEST_PATH;
}

START_TEST(test_simple_return) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |  ___    |   | | |         |   |"  //0
        "|   |   | |___| | |       | |___| |"  //1
        "| | | |_| |   | | | |    _| |   | |"  //2
        "| |_  |   | |_| | | |_  |   | |_| |"  //3
        "| __|_|___|_  | | | __|_|___|_  | |"  //4
        "| ____|  __ |_  | | ____|  __ |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|___|___________| |_______________|");//7
    place_mouse(4, 1, N, 4, 1);
    ck_assert_step( U, N, 4, 0, VALIDATE_SHORTEST_PATH);
    ck_assert_step( L, W, 3, 0, VALIDATE_SHORTEST_PATH);
    ck_assert_step( U, W, 2, 0, VALIDATE_SHORTEST_PATH);
    ck_assert_step( U, W, 1, 0, VALIDATE_SHORTEST_PATH);
    ck_assert_step( L, S, 1, 1, VALIDATE_SHORTEST_PATH);
    ck_assert_step( X, S, 1, 1, BACK_INTO_START);
    ck_assert_state(BACK_INTO_START);
}
END_TEST

Suite *validate_path_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Validate shortest path");
    tc_core = tcase_create("Core");
    tcase_add_unchecked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_simple_return);
    suite_add_tcase(s, tc_core);

    return s;
}

