#include <check.h>
#include "../src/a_star.h"

extern Open open;
void queue_reprioritize(uint8_t value);
void queue_push_priority(uint8_t value);
void mouse_init();

START_TEST(test_push) {
    queue_init(open);
    ck_assert_int_eq(open.n, -1);

    queue_push(open, 1);
    ck_assert_int_eq(open.n, 0);
    ck_assert_int_eq(open.data[0], 1);

    queue_push(open, 0);
    ck_assert_int_eq(open.n, 1);
    ck_assert_int_eq(open.data[1], 0);
}
END_TEST

START_TEST(test_pop) {
    queue_init(open);
    queue_push(open, 1);
    queue_push(open, 0);

    ck_assert_int_eq(open.n, 1);
    ck_assert_int_eq(queue_empty(open), 0);

    ck_assert_int_eq(queue_pop(open), 0);
    ck_assert_int_eq(open.n, 0);
    ck_assert_int_eq(queue_empty(open), 0);

    ck_assert_int_eq(queue_pop(open), 1);
    ck_assert_int_eq(open.n, -1);
    ck_assert_int_eq(queue_empty(open), 1);
}
END_TEST

START_TEST(test_queue_push_priority) {
	maze[0].f = 3;
	maze[1].f = 6;
	maze[2].f = 4;
	maze[3].f = 5;
	maze[4].f = 8;
    queue_init(open);
    queue_push(open, 1);
    queue_push(open, 2);

	queue_push_priority(4);
    ck_assert_uint_eq(open.n, 2);
    ck_assert_uint_eq(open.data[0], 4);
    ck_assert_uint_eq(open.data[1], 1);
    ck_assert_uint_eq(open.data[2], 2);

	queue_push_priority(3);
    ck_assert_uint_eq(open.n, 3);
    ck_assert_uint_eq(open.data[1], 1);
    ck_assert_uint_eq(open.data[2], 3);
    ck_assert_uint_eq(open.data[3], 2);

	queue_push_priority(0);
    ck_assert_uint_eq(open.n, 4);
    ck_assert_uint_eq(open.data[3], 2);
    ck_assert_uint_eq(open.data[4], 0);
}
END_TEST

START_TEST(test_queue_reprioritize) {
	mouse_init();
	maze[0].f = 3;
	maze[2].f = 4;
	maze[3].f = 5;
	maze[1].f = 6;
	maze[4].f = 8;
    queue_init(open);
    queue_push(open, 1);
    queue_push(open, 4);
    queue_push(open, 0);
    queue_push(open, 3);
    queue_push(open, 2);

    ck_assert_uint_eq(open.n, 4);
	queue_reprioritize(1);
    ck_assert_uint_eq(open.n, 4);
    ck_assert_uint_eq(open.data[0], 4);
    ck_assert_uint_eq(open.data[1], 1);
    ck_assert_uint_eq(open.data[2], 0);
    ck_assert_uint_eq(open.data[3], 3);
    ck_assert_uint_eq(open.data[4], 2);

	queue_reprioritize(2);
    ck_assert_uint_eq(open.n, 4);
    ck_assert_uint_eq(open.data[3], 3);
    ck_assert_uint_eq(open.data[4], 2);

	queue_reprioritize(0);
    ck_assert_uint_eq(open.n, 4);
    ck_assert_uint_eq(open.data[0], 4);
    ck_assert_uint_eq(open.data[1], 1);
    ck_assert_uint_eq(open.data[2], 3);
    ck_assert_uint_eq(open.data[3], 2);
    ck_assert_uint_eq(open.data[4], 0);
}
END_TEST

Suite *queue_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Queue");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_push);
    tcase_add_test(tc_core, test_pop);
    tcase_add_test(tc_core, test_queue_push_priority);
    tcase_add_test(tc_core, test_queue_reprioritize);
    suite_add_tcase(s, tc_core);

    return s;
}

