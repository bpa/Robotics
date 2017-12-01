#include <check.h>
#include "maze.h"

static void setup() {
    analogWrite(FRONT_SENSOR, 35);
    INIT();
    state = VALIDATE_SHORTEST_PATH;
}

START_TEST(test_1_1_W) {
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
    place_mouse(2, 1, W, 2, 1);
    ck_assert_step(X, W, 2, 1, RETURN_TO_START);
    ck_assert_step(U, W, 1, 1, RETURN_TO_START);
    ck_assert_step(U, W, 0, 1, BACK_INTO_START);
    ck_assert_step(L, S, 0, 2, BACK_INTO_START);
    ck_assert_step(D, S, 0, 1, BACK_INTO_START);
    ck_assert_step(H, S, 0, 0, RACE_TO_CENTER);
    ck_assert_state(BACK_INTO_START);
}
END_TEST

START_TEST(test_1_1_W_b) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |_____    |   | | |_____    |   |"  //0
        "|_   _    |___| | |_   _    |___| |"  //1
        "| | | |_| |   | | | | | |_| |   | |"  //2
        "| |_  |   | |_| | | |_  |   | |_| |"  //3
        "| ____|___|_  | | | ____|___|_  | |"  //4
        "| ____|  __ |_  | | ____|  __ |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|___|___________| |___|___________|");//7
    place_mouse(2, 1, W, 2, 1);
    ck_assert_step( X, W, 2, 1, RETURN_TO_START);
    ck_assert_step( U, W, 1, 1, RETURN_TO_START);
    ck_assert_step( L, S, 1, 2, BACK_INTO_START);
    ck_assert_step(DR, E, 1, 1, BACK_INTO_START);
    ck_assert_step(DL, S, 0, 1, BACK_INTO_START);
    ck_assert_step( H, S, 0, 0, RACE_TO_CENTER);
    ck_assert_state(BACK_INTO_START);
}
END_TEST

START_TEST(test_1_1_S) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| | ____    |   | | |  ___    |   |"  //0
        "|   __    |___| | |   __    |___| |"  //1
        "| | __|_| |   | | | | __|_| |   | |"  //2
        "| |_  |   | |_| | | |_  |   | |_| |"  //3
        "| __|_|___|_  | | | __|_|___|_  | |"  //4
        "| ____|  __ |_  | | ____|  __ |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|___|___________| |___|___________|");//7
    place_mouse(2, 0, W, 2, 0);
    ck_assert_step(X, W, 2, 0, RETURN_TO_START);
    ck_assert_step(U, W, 1, 0, RETURN_TO_START);
    ck_assert_step(L, S, 1, 1, RETURN_TO_START);
    ck_assert_step(R, W, 0, 1, BACK_INTO_START);
    ck_assert_step(L, S, 0, 2, BACK_INTO_START);
    ck_assert_step(D, S, 0, 1, BACK_INTO_START);
    ck_assert_step(H, S, 0, 0, RACE_TO_CENTER);
    ck_assert_state(BACK_INTO_START);
}
END_TEST

START_TEST(test_1_1_N) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| |    _    |   | | |    _    |   |"  //0
        "|   |_|   |___| | |   |_|   |___| |"  //1
        "| | __|_| |   | | | | __|_| |   | |"  //2
        "| |_  |   | |_| | | |_  |   | |_| |"  //3
        "| __|_|___|_  | | | __|_|___|_  | |"  //4
        "| ____|  __ |_  | | ____|  __ |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|___|___________| |___|___________|");//7
    place_mouse(1, 2, N, 1, 2);
    ck_assert_step(X, N, 1, 2, RETURN_TO_START);
    ck_assert_step(U, N, 1, 1, RETURN_TO_START);
    ck_assert_step(L, W, 0, 1, BACK_INTO_START);
    ck_assert_step(L, S, 0, 2, BACK_INTO_START);
    ck_assert_step(D, S, 0, 1, BACK_INTO_START);
    ck_assert_step(H, S, 0, 0, RACE_TO_CENTER);
    ck_assert_state(BACK_INTO_START);
}
END_TEST

START_TEST(test_1_1_E) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| | ____    |   | | | ____    |   |"  //0
        "|  ___  | |___| | |  ___  | |___| |"  //1
        "| |_____| |   | | | |_____| |   | |"  //2
        "| |_  |   | |_| | | |_  |   | |_| |"  //3
        "| __|_|___|_  | | | __|_|___|_  | |"  //4
        "| ____|  __ |_  | | ____|  __ |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|___|___________| |___|___________|");//7
    place_mouse(2, 1, E, 2, 1);
    ck_assert_step( X, E, 2, 1, RETURN_TO_START);
    ck_assert_step(DL, S, 1, 1, RETURN_TO_START);
    ck_assert_step( R, W, 0, 1, BACK_INTO_START);
    ck_assert_step( L, S, 0, 2, BACK_INTO_START);
    ck_assert_step( D, S, 0, 1, BACK_INTO_START);
    ck_assert_step( H, W, 0, 0, RACE_TO_CENTER);
    ck_assert_state(BACK_INTO_START);
}
END_TEST

START_TEST(test_0_1_N) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| | ____    |   | | | ____    |   |"  //0
        "|   __|   |___| | |   __|   |___| |"  //1
        "| | __|_| |   | | | | __|_| |   | |"  //2
        "| |_  |   | |_| | | |_  |   | |_| |"  //3
        "| __|_|___|_  | | | __|_|___|_  | |"  //4
        "| ____|  __ |_  | | ____|  __ |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|___|___________| |___|___________|");//7
    place_mouse(0, 1, N, 0, 1);
    ck_assert_step( X, N, 0, 1, RETURN_TO_START);
    ck_assert_step( R, E, 1, 1, RETURN_TO_START);
    ck_assert_step(DL, S, 0, 1, BACK_INTO_START);
    ck_assert_step( H, S, 0, 0, RACE_TO_CENTER);
    ck_assert_state(BACK_INTO_START);
}
END_TEST

START_TEST(test_0_1_N_b) {
    setup_mazes(
        //0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
        "_________________ _________________"
        "| | ____    |   | | | ____    |   |"  //0
        "| | __|   |___| | | | __|   |___| |"  //1
        "| | __|_| |   | | | | __|_| |   | |"  //2
        "| |_  |   | |_| | | |_  |   | |_| |"  //3
        "| __|_|___|_  | | | __|_|___|_  | |"  //4
        "| ____|  __ |_  | | ____|  __ |_  |"  //5
        "| ______| ____| | | ______| ____| |"  //6
        "|___|___________| |___|___________|");//7
    place_mouse(0, 5, N, 0, 5);
    ck_assert_step( X, N, 0, 5, RETURN_TO_START);
    ck_assert_step( U, N, 0, 4, RETURN_TO_START);
    ck_assert_step( R, E, 1, 4, BACK_INTO_START);
    ck_assert_step(DL, S, 0, 4, BACK_INTO_START);
    ck_assert_step( D, S, 0, 3, BACK_INTO_START);
    ck_assert_step( D, S, 0, 2, BACK_INTO_START);
    ck_assert_step( D, S, 0, 1, BACK_INTO_START);
    ck_assert_step( H, W, 0, 0, RACE_TO_CENTER);
    ck_assert_state(BACK_INTO_START);
}
END_TEST

Suite *return_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Return to start");
    tc_core = tcase_create("Core");
    tcase_add_unchecked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_1_1_W);
    tcase_add_test(tc_core, test_1_1_W_b);
    tcase_add_test(tc_core, test_1_1_S);
    tcase_add_test(tc_core, test_1_1_N);
    tcase_add_test(tc_core, test_1_1_E);
    tcase_add_test(tc_core, test_0_1_N);
    tcase_add_test(tc_core, test_0_1_N_b);
    suite_add_tcase(s, tc_core);

    return s;
}

