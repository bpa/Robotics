#include <stdlib.h>
#include <check.h>

extern Suite *astar_suite(void);
extern Suite *explore_suite(void);
extern Suite *maze_suite(void);
extern Suite *localization_suite(void);
extern Suite *movement_suite(void);
extern Suite *queue_suite(void);
extern Suite *race_suite(void);
extern Suite *return_suite(void);
extern Suite *validate_path_suite(void);

int main() {
    int number_failed;
    SRunner *sr;
    
    sr = srunner_create(queue_suite());
    srunner_add_suite(sr, astar_suite());
    srunner_add_suite(sr, explore_suite());
    srunner_add_suite(sr, maze_suite());
    srunner_add_suite(sr, localization_suite());
    srunner_add_suite(sr, movement_suite());
    srunner_add_suite(sr, race_suite());
    srunner_add_suite(sr, return_suite());
    srunner_add_suite(sr, validate_path_suite());
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
