#include <check.h>
#include "maze.h"

static void setup() {
    analogWrite(FRONT_SENSOR, 35);
    INIT();
    state = BACK_INTO_START;
}

START_TEST(test_run) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| | ____    |   | | |  ___    |   |"  //0
        "|    _    |___| | |    _    |___| |"  //1
        "| | | |_| |   | | | | | |_| |   | |"  //2
        "| |_  |   | |_| | | |_  |   | |_| |"  //3
        "| __|_|___|_  | | | __|_|___|_  | |"  //4
        "| ____|  __ |_  | | ____|  __ |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|___|___________| |___|___________|");//7
    place_mouse(0, 1, S, 0, 1);
    find_path(mouse.x, mouse.y, 0, 0, mouse.maze, mouse.current_path);
    ck_assert_step(H, S, 0, 0, RACE_TO_CENTER);
    ck_assert_step(U, S, 0, 1, RACE_TO_CENTER);
    ck_assert_step(L, E, 1, 1, RACE_TO_CENTER);
    ck_assert_step(U, E, 2, 1, RACE_TO_CENTER);
    ck_assert_step(U, E, 3, 1, RACE_TO_CENTER);
    ck_assert_step(U, E, 4, 1, RACE_TO_CENTER);
    ck_assert_step(R, S, 4, 2, RACE_TO_CENTER);
    ck_assert_step(U, S, 4, 3, RACE_TO_CENTER);
    ck_assert_step(U, S, 4, 4, DONE);
}
END_TEST

Suite *race_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Race to center");
    tc_core = tcase_create("Core");
    tcase_add_unchecked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_run);
    suite_add_tcase(s, tc_core);

    return s;
}

