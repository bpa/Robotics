#include <check.h>
#include "maze.h"

static void setup() {
    analogWrite(FRONT_SENSOR, 35);
    INIT();
}

START_TEST(test_no_go) {
    analogWrite(FRONT_SENSOR, 350);
    INIT();
    ck_assert_state(SENSE);
}
END_TEST

START_TEST(test_normal) {
    ck_assert_state(EXPLORE_TO_CENTER);
}
END_TEST

START_TEST(test_explore) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| | |__      _  | | |             |"  //0
        "|__ |___| |_|_  | |               |"  //1
        "|  _______  |   | |               |"  //2
        "| |   |   | | | | |               |"  //3
        "| | |_| __| | | | |               |"  //4
        "| |____   | | | | |               |"  //5
        "| __  __|_____| | |               |"  //6
        "|___|___________| |_______________|");//7
    place_mouse(0, 0, S, 4, 4);
    ck_assert_step(U, S, 0, 1, EXPLORE_TO_CENTER);
    ck_assert_step(L, E, 1, 1, EXPLORE_TO_CENTER);
    ck_assert_step(R, S, 1, 2, EXPLORE_TO_CENTER);
    ck_assert_step(L, E, 2, 2, EXPLORE_TO_CENTER);
    ck_assert_step(U, E, 3, 2, EXPLORE_TO_CENTER);
    ck_assert_step(U, E, 4, 2, EXPLORE_TO_CENTER);
    ck_assert_step(U, E, 5, 2, EXPLORE_TO_CENTER);
    ck_assert_step(R, S, 5, 3, EXPLORE_TO_CENTER);
    ck_assert_step(U, S, 5, 4, EXPLORE_TO_CENTER);
    ck_assert_step(U, S, 5, 5, EXPLORE_TO_CENTER);
    ck_assert_step(U, S, 5, 6, EXPLORE_TO_CENTER);
    ck_assert_step(R, W, 4, 6, EXPLORE_TO_CENTER);
    ck_assert_step(R, N, 4, 5, EXPLORE_TO_CENTER);
    ck_assert_step(L, W, 3, 5, EXPLORE_TO_CENTER);
    ck_assert_step(R, N, 3, 4, EXPLORE_TO_CENTER);
    ck_assert_step(R, E, 4, 4, VALIDATE_SHORTEST_PATH);
}
END_TEST

START_TEST(test_dead_end) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |  __     |   | | |             |"  //0
        "| | |__ | |___| | | |             |"  //1
        "| | |___|_|   | | | |             |"  //2
        "| |_  |   | |_| | | |             |"  //3
        "| __|_ ___|_  | | |               |"  //4
        "| ____|  _| |_  | |               |"  //5
        "| ______| ____| | |               |"  //6
        "|_______________| |_______________|");//7
    place_mouse(0, 3, S, 4, 4);
    ck_assert_step( U, S, 0, 4, EXPLORE_TO_CENTER);
    ck_assert_step( L, E, 1, 4, EXPLORE_TO_CENTER);
    ck_assert_step(DL, S, 0, 4, EXPLORE_TO_CENTER);
    ck_assert_step( U, S, 0, 5, EXPLORE_TO_CENTER);
}
END_TEST

START_TEST(test_longer_dead_end) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |  __     |   | | |             |"  //0
        "| | |__ | |___| | | |             |"  //1
        "| | |___|_|   | | | |             |"  //2
        "| |_  |   | |_| | | |_            |"  //3
        "| __|_ ___|_  | | | __|_          |"  //4
        "| ____|  _| |_  | | ____|         |"  //5
        "| ______| ____| | |               |"  //6
        "|_______________| |_______________|");//7
    place_mouse(2, 5, E, 4, 4);
    ck_assert_step( D, E, 1, 5, EXPLORE_TO_CENTER);
    ck_assert_step(DL, S, 0, 5, EXPLORE_TO_CENTER);
    ck_assert_step( U, S, 0, 6, EXPLORE_TO_CENTER);
}
END_TEST

START_TEST(test_back_opposite) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |  __     |   | | |         |   |"  //0
        "|   |__ | |___| | |    __ | |___| |"  //1
        "| | |___|_|   | | |   |___|_|   | |"  //2
        "| |_  |   | |_| | | __      | |_| |"  //3
        "| __|_ ___|_  | | | __|_ ___|_  | |"  //4
        "| ____|  _| |_  | | ____|  _| |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|_______________| |_______________|");//7
    place_mouse(1, 4, E, 4, 4);
    ck_assert_step(DR, N, 0, 4, EXPLORE_TO_CENTER);
    ck_assert_step( U, N, 0, 3, EXPLORE_TO_CENTER);
}
END_TEST

START_TEST(test_one_twist) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |  __     |   | | |         |   |"  //0
        "| | |__ | |___| | | |  __ | |___| |"  //1
        "| | |___|_|   | | | | |___|_|   | |"  //2
        "| |_  |   | |_| | | |_      | |_| |"  //3
        "| __|_ ___|_  | | | __|_ ___|_  | |"  //4
        "| ____|  _| |_  | | ____|  _| |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|_______________| |_______________|");//7
    place_mouse(4, 5, E, 4, 4);
    ck_assert_step(DR, N, 3, 5, EXPLORE_TO_CENTER);
    ck_assert_step(DL, E, 3, 6, EXPLORE_TO_CENTER);
    ck_assert_step( D, E, 2, 6, EXPLORE_TO_CENTER);
    ck_assert_step( D, E, 1, 6, EXPLORE_TO_CENTER);
    ck_assert_step(DL, S, 0, 6, EXPLORE_TO_CENTER);
}
END_TEST

START_TEST(test_tee) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |  __     |   | | |             |"  //0
        "| | |__ | |___| | | |             |"  //1
        "| | |___|_|   | | | |             |"  //2
        "| |_  |   | |_| | | |_            |"  //3
        "| __|_ ___|_  | | | __|_ ___      |"  //4
        "| ____|  _| |_  | | ____|  _|     |"  //5
        "| ______| ____| | | ______|       |"  //6
        "|_______________| |_______________|");//7
    place_mouse(4, 7, E, 4, 4);
    ck_assert_step( L, N, 4, 6, EXPLORE_TO_CENTER);
    ck_assert_step( R, E, 5, 6, EXPLORE_TO_CENTER);
    ck_assert_step( L, N, 5, 5, EXPLORE_TO_CENTER);
    ck_assert_step(DL, E, 5, 6, EXPLORE_TO_CENTER);
    ck_assert_step( U, E, 6, 6, EXPLORE_TO_CENTER);
    ck_assert_step(DL, S, 5, 6, EXPLORE_TO_CENTER);
    ck_assert_step( R, W, 4, 6, EXPLORE_TO_CENTER);
    ck_assert_step( L, S, 4, 7, EXPLORE_TO_CENTER);
    ck_assert_step( L, E, 5, 7, EXPLORE_TO_CENTER);
}
END_TEST

START_TEST(test_back_swirl) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |  __     |   | | |             |"  //0
        "| | |__ | |___| | | |        ___  |"  //1
        "| | |___|_|   | | | |       |   | |"  //2
        "| |_  |   | |_| | | |_      | |_| |"  //3
        "| __|_ ___|_  | | | __|_ ___|_  | |"  //4
        "| ____|  _| |_  | | ____|  _| |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|_______________| |_______________|");//7
    place_mouse(6, 3, S, 4, 4);
    ck_assert_step(DR, E, 6, 2, EXPLORE_TO_CENTER);
    ck_assert_step(DR, N, 5, 2, EXPLORE_TO_CENTER);
    ck_assert_step( D, N, 5, 3, EXPLORE_TO_CENTER);
    ck_assert_step(DR, W, 5, 4, EXPLORE_TO_CENTER);
    ck_assert_step(DL, N, 6, 4, EXPLORE_TO_CENTER);
    ck_assert_step(DR, W, 6, 5, EXPLORE_TO_CENTER);
    ck_assert_step(DL, N, 7, 5, EXPLORE_TO_CENTER);
    ck_assert_step( U, N, 7, 4, EXPLORE_TO_CENTER);
}
END_TEST

START_TEST(test_back_past_turn) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |  __     |   | | |         |   |"  //0
        "| | |__ | |___| | | |  __ | |___| |"  //1
        "| | |___|_|   | | | | |___|_|   | |"  //2
        "| |_  |   | |_| | | |_      | |_| |"  //3
        "| __|_ ___|_  | | | __|_ ___|_  | |"  //4
        "| ____|  _| |_  | | ____|  _| |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|_______________| |_______________|");//7
    place_mouse(2, 2, W, 4, 4);
    ck_assert_step(DR, S, 3, 2, EXPLORE_TO_CENTER);
    ck_assert_step( D, S, 3, 1, EXPLORE_TO_CENTER);
    ck_assert_step( R, W, 2, 1, EXPLORE_TO_CENTER);
}
END_TEST

Suite *explore_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Explore");
    tc_core = tcase_create("Core");
    tcase_add_unchecked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_no_go);
    tcase_add_test(tc_core, test_normal);
    tcase_add_test(tc_core, test_explore);
    tcase_add_test(tc_core, test_dead_end);
    tcase_add_test(tc_core, test_longer_dead_end);
    tcase_add_test(tc_core, test_back_opposite);
    tcase_add_test(tc_core, test_one_twist);
    tcase_add_test(tc_core, test_tee);
    tcase_add_test(tc_core, test_back_swirl);
    tcase_add_test(tc_core, test_back_past_turn);
    suite_add_tcase(s, tc_core);

    return s;
}

