#include <check.h>
#include "../src/queue.h"

static Maze maze;
static PriorityQueue q;
void mouse_init();

static void setup() {
    for (int i=1; i<CELLS; i++) {
        maze[i].maze_ind = i;
        maze[i].f = i;
    }
}

START_TEST(test_push) {
    queue_init(q);
    ck_assert_int_eq(q.size, -1);

    queue_push(q, &maze[1]);
    ck_assert_int_eq(q.size, 0);
    ck_assert_int_eq(q.data[0]->f, 1);

    queue_push(q, &maze[0]);
    ck_assert_int_eq(q.size, 1);
    ck_assert_int_eq(q.data[1]->f, 0);
}
END_TEST

START_TEST(test_pop) {
    queue_init(q);
    queue_push(q, &maze[1]);
    queue_push(q, &maze[0]);

    ck_assert_int_eq(q.size, 1);
    ck_assert_int_eq(queue_empty(q), 0);

    ck_assert_ptr_eq(queue_pop(q), &maze[0]);
    ck_assert_int_eq(q.size, 0);
    ck_assert_int_eq(queue_empty(q), 0);

    ck_assert_ptr_eq(queue_pop(q), &maze[1]);
    ck_assert_int_eq(q.size, -1);
    ck_assert_int_eq(queue_empty(q), 1);
}
END_TEST

START_TEST(test_queue_push_priority) {
    queue_init(q);
    queue_push(q, &maze[3]);
    queue_push(q, &maze[1]);

	queue_push_priority(q, &maze[4]);
    ck_assert_uint_eq(q.size, 2);
    ck_assert_uint_eq(q.data[0]->f, 4);
    ck_assert_uint_eq(q.data[1]->f, 3);
    ck_assert_uint_eq(q.data[2]->f, 1);

	queue_push_priority(q, &maze[2]);
    ck_assert_uint_eq(q.size, 3);
    ck_assert_uint_eq(q.data[1]->f, 3);
    ck_assert_uint_eq(q.data[2]->f, 2);
    ck_assert_uint_eq(q.data[3]->f, 1);

	queue_push_priority(q, &maze[0]);
    ck_assert_uint_eq(q.size, 4);
    ck_assert_uint_eq(q.data[3]->f, 1);
    ck_assert_uint_eq(q.data[4]->f, 0);
}
END_TEST

START_TEST(test_queue_reprioritize) {
	mouse_init();
    queue_init(q);
    queue_push(q, &maze[9]);
    queue_push(q, &maze[6]);
    queue_push(q, &maze[3]);
    queue_push(q, &maze[1]);

    ck_assert_uint_eq(q.size, 3);
    maze[9].f = 5;
	queue_reprioritize(q, &maze[9]);
    ck_assert_uint_eq(q.size, 3);
    ck_assert_uint_eq(q.data[0]->f, 6);
    ck_assert_uint_eq(q.data[1]->f, 5);
    ck_assert_uint_eq(q.data[2]->f, 3);
    ck_assert_uint_eq(q.data[3]->f, 1);
    ck_assert_uint_eq(q.data[1]->maze_ind, 9);

    maze[3].f = 2;
	queue_reprioritize(q, &maze[2]);
    ck_assert_uint_eq(q.size, 3);
    ck_assert_uint_eq(q.data[2]->f, 2);
    ck_assert_uint_eq(q.data[3]->f, 1);
    ck_assert_uint_eq(q.data[2]->maze_ind, 3);

    maze[9].f = 0;
	queue_reprioritize(q, &maze[9]);
    ck_assert_uint_eq(q.size, 3);
    ck_assert_uint_eq(q.data[0]->f, 6);
    ck_assert_uint_eq(q.data[1]->f, 2);
    ck_assert_uint_eq(q.data[2]->f, 1);
    ck_assert_uint_eq(q.data[3]->f, 0);

	queue_reprioritize(q, &maze[2]);
    ck_assert_uint_eq(q.size, 3);
    ck_assert_uint_eq(q.data[0]->f, 6);
    ck_assert_uint_eq(q.data[1]->f, 2);
    ck_assert_uint_eq(q.data[2]->f, 1);
    ck_assert_uint_eq(q.data[3]->f, 0);
}
END_TEST

Suite *queue_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Queue");
    tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, setup, NULL);

    tcase_add_test(tc_core, test_push);
    tcase_add_test(tc_core, test_pop);
    tcase_add_test(tc_core, test_queue_push_priority);
    tcase_add_test(tc_core, test_queue_reprioritize);
    suite_add_tcase(s, tc_core);

    return s;
}

