#include <check.h>
#include "maze.h"
#include "../src/a_star.h"
#include "../src/state.h"
#include "../src/hardware.h"
#include "../src/movement.h"

#define ck_assert_step(W, D, X, Y, S) do { \
    Wall _f = (W); \
    Direction _d = (D); \
    intmax_t _x = (X); \
    intmax_t _y = (Y); \
    void (*_p)() = (S); \
    uint8_t _w = cell(hidden, cx, cy).walls; \
    analogWrite(FRONT_SENSOR, (_w & visible[looking][0]) ? 200 : 10); \
    analogWrite(LEFT_SENSOR,  (_w & visible[looking][1]) ? 200 : 10); \
    analogWrite(RIGHT_SENSOR, (_w & visible[looking][2]) ? 200 : 10); \
    near_target = true; \
    state(); \
    ck_assert_msg(_d == facing, "Assertion '%s' failed: %d != %d", "direction == "#D, facing, _d); \
    ck_assert_msg(_x == x && _y == y, "Assertion '%s' failed: (%d, %d) != (%d, %d)", "("#X","#Y") == loc", _x, _y, x, y); \
    ck_assert_msg(state == _p, "Assertion '%s' failed: %d != %d", "state == "#S, state, _p); \
    looking = D; \
    cx = _x; \
    cy = _y; \
} while (0)

extern int x, y;
extern Direction moving;
extern Wall visible[][4];

static Path path;
static Cell hidden[MAZE*MAZE];

char hard_maze[] =
//0 1 2 3 4 5 6 7
"_________________"
"| | |__      _  |" //0
"|__ |___| |_|_  |" //1
"|  _______  |   |" //2
"| |   |   | | | |" //3
"| | |_| __| | | |" //4
"| |____   | | | |" //5
"| __  __|_____| |" //6
"|___|___________|";//7

void hard_maze_setup() {
    create_maze(hard_maze, hidden);
}

void setup() {
    state = INIT;
    analogWrite(FRONT_SENSOR, 35);
}

START_TEST(test_no_go) {
    analogWrite(FRONT_SENSOR, 350);
    state();
    ck_assert_state(DONE);
}
END_TEST

START_TEST(test_normal) {
    state();
    ck_assert_state(EXPLORE_TO_CENTER);
}
END_TEST

START_TEST(test_explore) {
    hard_maze_setup();
    state();
    uint8_t looking = S;
    int cx=0, cy=0;
    ck_assert_state(EXPLORE_TO_CENTER);
    ck_assert_step(U, S, 0, 1, EXPLORE_TO_CENTER);
    ck_assert_step(L, E, 0, 1, EXPLORE_TO_CENTER);
    ck_assert_step(U, E, 1, 1, EXPLORE_TO_CENTER);
    ck_assert_step(R, S, 1, 1, EXPLORE_TO_CENTER);
    ck_assert_step(U, S, 1, 2, EXPLORE_TO_CENTER);
    ck_assert_step(L, E, 1, 2, EXPLORE_TO_CENTER);
    ck_assert_step(U, E, 2, 2, EXPLORE_TO_CENTER);
    ck_assert_step(U, E, 3, 2, EXPLORE_TO_CENTER);
    ck_assert_step(U, E, 4, 2, EXPLORE_TO_CENTER);
    ck_assert_step(U, E, 5, 2, EXPLORE_TO_CENTER);
    ck_assert_step(R, S, 5, 2, EXPLORE_TO_CENTER);
    ck_assert_step(U, S, 5, 3, EXPLORE_TO_CENTER);
    ck_assert_step(U, S, 5, 4, EXPLORE_TO_CENTER);
    ck_assert_step(U, S, 5, 5, EXPLORE_TO_CENTER);
    ck_assert_step(U, S, 5, 6, EXPLORE_TO_CENTER);
    ck_assert_step(R, W, 5, 6, EXPLORE_TO_CENTER);
    ck_assert_step(U, W, 4, 6, EXPLORE_TO_CENTER);
    ck_assert_step(R, N, 4, 6, EXPLORE_TO_CENTER);
    ck_assert_step(U, N, 4, 5, EXPLORE_TO_CENTER);
    ck_assert_step(L, W, 4, 5, EXPLORE_TO_CENTER);
    ck_assert_step(U, W, 3, 5, EXPLORE_TO_CENTER);
    ck_assert_step(R, N, 3, 5, EXPLORE_TO_CENTER);
    ck_assert_step(U, N, 3, 4, VALIDATE_SHORTEST_PATH);

/* Another test for another day
    ck_assert_step(D, N, 3, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step(L, W, 3, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, W, 2, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, W, 1, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step(R, N, 1, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, N, 1, 4, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, N, 1, 3, VALIDATE_SHORTEST_PATH);
    ck_assert_step(D, N, 1, 4, VALIDATE_SHORTEST_PATH);
    ck_assert_step(D, N, 1, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step(R, E, 1, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, E, 2, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, E, 3, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step(R, S, 3, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, S, 3, 6, VALIDATE_SHORTEST_PATH);
    ck_assert_step(R, W, 3, 6, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, W, 2, 6, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, W, 1, 6, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, W, 0, 6, VALIDATE_SHORTEST_PATH);
    ck_assert_step(R, N, 0, 6, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, N, 0, 5, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, N, 0, 4, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, N, 0, 3, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, N, 0, 2, VALIDATE_SHORTEST_PATH);
    ck_assert_step(R, E, 0, 2, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, E, 1, 2, VALIDATE_SHORTEST_PATH);
    ck_assert_step(L, N, 1, 2, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, N, 1, 1, VALIDATE_SHORTEST_PATH);
    ck_assert_step(L, W, 1, 1, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, W, 0, 1, VALIDATE_SHORTEST_PATH);
    ck_assert_step(R, N, 0, 1, VALIDATE_SHORTEST_PATH);
    ck_assert_step(U, N, 0, 0, RACE_TO_CENTER);
    ck_assert_step(D, N, 0, 1, RACE_TO_CENTER);
    ck_assert_step(R, E, 0, 1, RACE_TO_CENTER);
    */
}
END_TEST

Suite *run_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Run");
    tc_core = tcase_create("Core");
    tcase_add_unchecked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_no_go);
    tcase_add_test(tc_core, test_normal);
    tcase_add_test(tc_core, test_explore);
    suite_add_tcase(s, tc_core);

    return s;
}

