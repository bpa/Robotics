#include <check.h>
#include "maze.h"

static void setup() {
    analogWrite(FRONT_SENSOR, 35);
    INIT();
    state = VALIDATE_SHORTEST_PATH;
}

START_TEST(test_no_check_required) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |  ___    |   | | |  ___        |"  //0
        "|   |   | |___| | |   |   | |     |"  //1
        "| | | |_| |   | | |      _| |     |"  //2
        "| |_  |   | |_| | |     |   |     |"  //3
        "| __|_|___|_  | | |     |___|     |"  //4
        "| ____|  __ |_  | |               |"  //5
        "| ______| ____| | |               |"  //6
        "|___|___________| |_______________|");//7
    state = EXPLORE_TO_CENTER;
    place_mouse(4, 3, S, 4, 4);
    ck_assert_step(U, S, 4, 4, VALIDATE_SHORTEST_PATH);
    ck_assert_step(X, S, 4, 4, RETURN_TO_START);
}
END_TEST

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
    ck_assert_step(U, N, 4, 0, VALIDATE_SHORTEST_PATH);
    ck_assert_step(L, W, 3, 0, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, W, 2, 0, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, W, 1, 0, VALIDATE_SHORTEST_PATH);
    ck_assert_step(L, S, 1, 1, VALIDATE_SHORTEST_PATH);
    ck_assert_step(X, S, 1, 1, RETURN_TO_START);
}
END_TEST

START_TEST(test_backtrack) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |     |   |   | | |             |"  //0
        "|   | |___|___| | |               |"  //1
        "| | |_ ___|   | | | |    ___      |"  //2
        "| |_  |     |_| | | |_  |     |_  |"  //3
        "| __| |___|__ | | | __| |___|__ | |"  //4
        "| __|______ __  | | __|______ __  |"  //5
        "| _______  ___| | | ______  ____| |"  //6
        "|___|___________| |_______________|");//7
    place_mouse(4, 4, S, 4, 4);
    ck_assert_step(DR, E, 4, 3, VALIDATE_SHORTEST_PATH);
    ck_assert_step( U, E, 5, 3, VALIDATE_SHORTEST_PATH);
    ck_assert_step( L, N, 5, 2, VALIDATE_SHORTEST_PATH);
    ck_assert_step( R, E, 6, 2, VALIDATE_SHORTEST_PATH);
    ck_assert_step( R, S, 6, 3, VALIDATE_SHORTEST_PATH);
    ck_assert_step(DR, E, 6, 2, VALIDATE_SHORTEST_PATH);
    ck_assert_step(DR, N, 5, 2, VALIDATE_SHORTEST_PATH);
    ck_assert_step(DL, E, 5, 3, VALIDATE_SHORTEST_PATH);
    ck_assert_step( R, S, 5, 4, VALIDATE_SHORTEST_PATH);
    ck_assert_step( L, E, 6, 4, VALIDATE_SHORTEST_PATH);
    ck_assert_step( R, S, 6, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step( R, W, 5, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step( U, W, 4, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step( U, W, 3, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step( U, W, 2, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step( R, N, 2, 4, VALIDATE_SHORTEST_PATH);
    ck_assert_step( U, N, 2, 3, VALIDATE_SHORTEST_PATH);
    ck_assert_step( L, W, 1, 3, VALIDATE_SHORTEST_PATH);
    ck_assert_step( R, N, 1, 2, VALIDATE_SHORTEST_PATH);
    ck_assert_step( U, N, 1, 1, VALIDATE_SHORTEST_PATH);
    ck_assert_step( X, N, 1, 1, RETURN_TO_START);
}
END_TEST

Suite *validate_path_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Validate shortest path");
    tc_core = tcase_create("Core");
    tcase_add_unchecked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_no_check_required);
    tcase_add_test(tc_core, test_simple_return);
    tcase_add_test(tc_core, test_backtrack);
    suite_add_tcase(s, tc_core);

    return s;
}

