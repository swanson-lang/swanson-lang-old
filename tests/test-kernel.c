/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <check.h>

#include "swanson/kernel.h"
#include "swanson/metamodel.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Helper functions
 */

#define check_operation(kind, obj, name) \
    do { \
        struct swan_operation  *operation; \
        operation = swan_##kind##_get_operation(obj, name); \
        fail_if(operation == NULL, \
                #kind " should have operation named %s", name); \
    } while (0)

#define check_unknown_operation(kind, obj) \
    do { \
        struct swan_operation  *operation; \
        operation = swan_##kind##_get_operation(obj, "unknown!"); \
        fail_if(operation != NULL, \
                #kind " should not have operation named \"unknown!\""); \
    } while (0)


/*-----------------------------------------------------------------------
 * Static memory management methods
 */

START_TEST(test__static__operations)
{
    DESCRIBE_TEST;
    check_operation(opset, swan_static__opset(), "~alias");
    check_operation(opset, swan_static__opset(), "~unref");
    check_unknown_operation(opset, swan_static__opset());
}
END_TEST


/*-----------------------------------------------------------------------
 * Sizes
 */

START_TEST(test__size__operations)
{
    DESCRIBE_TEST;

    check_operation(opset, swan_size__ro__static__opset(), "~alias");
    check_operation(opset, swan_size__ro__static__opset(), "~unref");
    check_operation(opset, swan_size__ro__static__opset(), "print");
    check_unknown_operation(opset, swan_size__ro__static__opset());

    check_operation(opset, swan_size__ro__explicit__opset(), "~alloc");
    check_operation(opset, swan_size__ro__explicit__opset(), "~unref");
    check_operation(opset, swan_size__ro__explicit__opset(), "print");
    check_unknown_operation(opset, swan_size__ro__explicit__opset());

    check_operation(opset, swan_size__rw__static__opset(), "~alias");
    check_operation(opset, swan_size__rw__static__opset(), "~unref");
    check_operation(opset, swan_size__rw__static__opset(), "print");
    check_unknown_operation(opset, swan_size__rw__static__opset());

    check_operation(opset, swan_size__rw__explicit__opset(), "~alloc");
    check_operation(opset, swan_size__rw__explicit__opset(), "~unref");
    check_operation(opset, swan_size__rw__explicit__opset(), "print");
    check_unknown_operation(opset, swan_size__rw__explicit__opset());
}
END_TEST

START_TEST(test__size__add)
{
    DESCRIBE_TEST;

    size_t  *result;
    struct swan_value  params[3] =
    { SWAN_VALUE_EMPTY, SWAN_VALUE_EMPTY, SWAN_VALUE_EMPTY };

    swan_size_new(&params[1], 2);
    swan_size_new(&params[2], 3);
    fail_if_error(swan_value_evaluate(&params[1], "+", 3, params));
    result = params[0].content;
    fail_unless(*result == 5,
                "Unexpected result: got %zu, expected %zu",
                *result, (size_t) 5);

    fail_if_error(swan_value_unref(&params[0]));
    fail_if_error(swan_value_unref(&params[1]));
    fail_if_error(swan_value_unref(&params[2]));
}
END_TEST

START_TEST(test__size__assign)
{
    DESCRIBE_TEST;

    size_t  *result;
    struct swan_value  params[2] =
    { SWAN_VALUE_EMPTY, SWAN_VALUE_EMPTY };

    swan_size_new(&params[0], 2);
    swan_size_new(&params[1], 3);
    fail_if_error(swan_value_evaluate(&params[0], "=", 2, params));
    result = params[0].content;
    fail_unless(*result == 3,
                "Unexpected result: got %zu, expected %zu",
                *result, (size_t) 3);

    fail_if_error(swan_value_unref(&params[0]));
    fail_if_error(swan_value_unref(&params[1]));
}
END_TEST

START_TEST(test__size__assign_add)
{
    DESCRIBE_TEST;

    size_t  *result;
    struct swan_value  params[2] =
    { SWAN_VALUE_EMPTY, SWAN_VALUE_EMPTY };

    swan_size_new(&params[0], 2);
    swan_size_new(&params[1], 3);
    fail_if_error(swan_value_evaluate(&params[0], "+=", 2, params));
    result = params[0].content;
    fail_unless(*result == 5,
                "Unexpected result: got %zu, expected %zu",
                *result, (size_t) 5);

    fail_if_error(swan_value_unref(&params[0]));
    fail_if_error(swan_value_unref(&params[1]));
}
END_TEST


/*-----------------------------------------------------------------------
 * The kernel
 */

START_TEST(test__kernel__operations)
{
    DESCRIBE_TEST;
    struct swan_value  kernel;
    swan_kernel_get(&kernel);
    check_operation(value, &kernel, "sizeof");
    check_unknown_operation(value, &kernel);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("kernel");

    TCase  *tc_static = tcase_create("static");
    tcase_add_test(tc_static, test__static__operations);
    suite_add_tcase(s, tc_static);

    TCase  *tc_size = tcase_create("size");
    tcase_add_test(tc_size, test__size__operations);
    tcase_add_test(tc_size, test__size__add);
    tcase_add_test(tc_size, test__size__assign);
    tcase_add_test(tc_size, test__size__assign_add);
    suite_add_tcase(s, tc_size);

    TCase  *tc_kernel = tcase_create("kernel");
    tcase_add_test(tc_kernel, test__kernel__operations);
    suite_add_tcase(s, tc_kernel);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = test_suite();
    SRunner  *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
