#include <check.h>
#include "maze.h"
#include "../src/a_star.h"
#include "../src/state.h"
#include "../src/hardware.h"
#include "../src/movement.h"

#define place_mouse(X, Y, D, X2, Y2) \
    cx=X; \
    cy=Y; \
    looking = D; \
    mouse.x = X; \
    mouse.y = Y; \
    mouse.facing = D; \
    find_path(mouse.x, mouse.y, X2, Y2, mouse.maze, mouse.shortest_path);

#define ck_assert_step(W, D, X, Y, S) do { \
    update_mouse(X, Y); \
    Wall _f = (W); \
    Direction _d = (D); \
    intmax_t _x = (X); \
    intmax_t _y = (Y); \
    void (*_p)() = (S); \
    ck_assert_msg(_f == moving, "Assertion '%s' failed: %s != %s", "moving == "#W, wall(moving), wall(_f)); \
    ck_assert_msg(_d == mouse.facing, "Assertion '%s' failed: %d != %d", "direction == "#D, mouse.facing, _d); \
    ck_assert_msg(_x == mouse.x && _y == mouse.y, "Assertion '%s' failed: (%d, %d) != (%d, %d)", "("#X","#Y") == loc", _x, _y, mouse.x, mouse.y); \
    ck_assert_msg(state == _p, "Assertion '%s' failed: %d != %d", "state == "#S, state, _p); \
    looking = D; \
    ck_assert_maze_subset(hidden, mouse.maze); \
} while (0)

extern Wall moving;
extern Mouse mouse;
extern Wall visible[][4];

static Path path;
static Maze hidden;
static Direction looking;
static int cx, cy;

void update_mouse(int x, int y) {
    uint8_t _w = cell(hidden, cx, cy).walls;
    analogWrite(FRONT_SENSOR, (_w & visible[looking][N]) ? 200 : 10);
    analogWrite(RIGHT_SENSOR, (_w & visible[looking][E]) ? 200 : 10);
    analogWrite(LEFT_SENSOR,  (_w & visible[looking][W]) ? 200 : 10);
    near_target = true;
    state();
    cx = x;
    cy = y;
}

static void setup() {
    create_maze(hidden, empty_maze);
    state = INIT;
    analogWrite(FRONT_SENSOR, 35);
    state();
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
    create_maze(hidden,
        //0 1 2 3 4 5 6 7
        "_________________"
        "| | |__      _  |"  //0
        "|__ |___| |_|_  |"  //1
        "|  _______  |   |"  //2
        "| |   |   | | | |"  //3
        "| | |_| __| | | |"  //4
        "| |____   | | | |"  //5
        "| __  __|_____| |"  //6
        "|___|___________|");//7
    place_mouse(0, 0, S, 4, 4);
    ck_assert_state(EXPLORE_TO_CENTER);
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
    setup_mazes(hidden, mouse.maze,
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
    setup_mazes(hidden, mouse.maze,
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
    setup_mazes(hidden, mouse.maze,
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

START_TEST(test_back_past_turn) {
    setup_mazes(hidden, mouse.maze,
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

Suite *run_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Run");
    tc_core = tcase_create("Core");
    tcase_add_unchecked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_no_go);
    tcase_add_test(tc_core, test_normal);
    tcase_add_test(tc_core, test_explore);
    tcase_add_test(tc_core, test_dead_end);
    tcase_add_test(tc_core, test_longer_dead_end);
    tcase_add_test(tc_core, test_back_opposite);
    tcase_add_test(tc_core, test_back_past_turn);
    suite_add_tcase(s, tc_core);

    return s;
}

