/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <check.h>

#include "s0.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Formal parameter sets
 */

/* TODO: Eventually verify the pre and post types */
static void
check_formal_param(struct s0_formals *formals, const char *param_name)
{
    const struct s0_formal  *formal;
    fail_if_error(formal = s0_formals_require(formals, param_name));
    fail_unless_streq("Parameter names", param_name, formal->name);
}

static void
check_missing_formal_param(struct s0_formals *formals, const char *param_name)
{
    fail_unless_error(s0_formals_require(formals, param_name));
}

START_TEST(test_formals_require)
{
    struct s0_formals  *formals;
    DESCRIBE_TEST;
    formals = s0_formals_new("test");
    fail_if_error(s0_formals_add(formals, "lhs", NULL, NULL));
    fail_if_error(s0_formals_add(formals, "rhs", NULL, NULL));
    fail_if_error(s0_formals_add(formals, "result", NULL, NULL));
    check_formal_param(formals, "lhs");
    check_formal_param(formals, "rhs");
    check_formal_param(formals, "result");
    check_missing_formal_param(formals, "nope");
    s0_formals_free(formals);
}
END_TEST

START_TEST(test_formals_redefined)
{
    struct s0_formals  *formals;
    DESCRIBE_TEST;
    formals = s0_formals_new("test");
    fail_if_error(s0_formals_add(formals, "lhs", NULL, NULL));
    fail_unless_error(s0_formals_add(formals, "lhs", NULL, NULL));
    s0_formals_free(formals);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("ast");

    TCase  *tc_ast = tcase_create("ast");
    tcase_add_test(tc_ast, test_formals_require);
    tcase_add_test(tc_ast, test_formals_redefined);
    suite_add_tcase(s, tc_ast);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = test_suite();
    SRunner  *runner = srunner_create(suite);

    initialize_tests();
    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    finalize_tests();

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
