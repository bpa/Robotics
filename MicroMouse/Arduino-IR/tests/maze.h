#ifndef TEST_MAZE
#define TEST_MAZE

#include "../src/state.h"
#include "../src/Mouse.h"

#define ck_assert_path_coord(X, Y, P) do { \
    intmax_t _x = (X); \
    intmax_t _y = (Y); \
    Pair _p = (P); \
    ck_assert_msg(_x == _p.x && _y == _p.y, "Assertion '%s' failed: (%d, %d) != (%d, %d)", "("#X","#Y") == "#P, _x, _y, _p.x, _p.y); \
} while (0)

#define ck_assert_state(S) do { \
    void (*_p)() = (S); \
    ck_assert_msg(state == _p, "Assertion '%s' failed: (%d) != (%d)", "state == "#S, state, _p); \
} while (0)

extern void create_maze(const char *maze_text, Cell *maze);

#endif
