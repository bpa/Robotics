#include <check.h>
#include "../src/a_star.h"
#include "maze.h"

char test_maze[] =
//0 1 2 3 4 5 6 7
"_________________"
"| | |_       _  |" //0
"|  _|  _| |_|_  |" //1
"| |_   _|_  |   |" //2
"|  _| |   | | | |" //3
"|   | |_____| | |" //4
"| | |  _    | | |" //5
"| |_  |_  |___| |" //6
"|___|___|_______|";//7

static Path path;
static Maze maze;

START_TEST(test_empty) {
    create_maze(maze, empty_maze);
    find_path(0, 0, 4, 4, maze, path);
    ck_assert_uint_eq(7, path.size);
    ck_assert_path_coord(4, 4, path.data[0]);
    ck_assert_path_coord(0, 1, path.data[7]);
}
END_TEST

START_TEST(test_next_cell) {
    create_maze(maze, test_maze);
    find_path(0, 0, 0, 1, maze, path);
    ck_assert_uint_eq(0, path.size);
    ck_assert_path_coord(0, 1, path.data[0]);
}
END_TEST

START_TEST(test_around_a_wall) {
    create_maze(maze, test_maze);
    find_path(0, 0, 1, 0, maze, path);
    ck_assert_uint_eq(2, path.size);
    ck_assert_path_coord(0, 1, path.data[2]);
    ck_assert_path_coord(1, 1, path.data[1]);
    ck_assert_path_coord(1, 0, path.data[0]);
}
END_TEST

START_TEST(test_back_way) {
    create_maze(maze, test_maze);
    find_path(5, 5, 7, 6, maze, path);
    ck_assert_uint_eq(6, path.size);
    ck_assert_path_coord(4, 5, path.data[6]);
    ck_assert_path_coord(4, 6, path.data[5]);
    ck_assert_path_coord(4, 7, path.data[4]);
    ck_assert_path_coord(5, 7, path.data[3]);
    ck_assert_path_coord(6, 7, path.data[2]);
    ck_assert_path_coord(7, 7, path.data[1]);
    ck_assert_path_coord(7, 6, path.data[0]);
}
END_TEST

START_TEST(test_to_center) {
    create_maze(maze, test_maze);
    find_path(0, 0, 4, 4, maze, path);
    ck_assert_uint_eq(21, path.size);
}
END_TEST

Suite *astar_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("A*");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_empty);
    tcase_add_test(tc_core, test_next_cell);
    tcase_add_test(tc_core, test_around_a_wall);
    tcase_add_test(tc_core, test_back_way);
    tcase_add_test(tc_core, test_to_center);
    suite_add_tcase(s, tc_core);

    return s;
}

