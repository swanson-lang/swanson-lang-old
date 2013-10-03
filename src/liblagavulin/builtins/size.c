/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <libcork/core.h>
#include <libcork/helpers/errors.h>

#include "lagavulin.h"


static int
lgv_size__unref(void *user_data, size_t param_count,
                struct lgv_parameter *params)
{
    struct lgv_ref  *ref;
    rip_check(ref = lgv_parameter_get_ref("ref", param_count, params));
    lgv_simple_method_set_free(lgv_size, ref->methods);
    free(ref->value);
    return 0;
}

static int
lgv_size__add(void *user_data, size_t param_count,
              struct lgv_parameter *params)
{
    struct lgv_ref  *result;
    size_t  *lhs;
    size_t  *rhs;
    rip_check(result = lgv_parameter_get_ref("result", param_count, params));
    rip_check(lhs = lgv_parameter_get_value("lhs", param_count, params));
    rip_check(rhs = lgv_parameter_get_value("rhs", param_count, params));
    return lgv_ref_new_size(result, *lhs + *rhs);
}


lgv_define_simple_method_set(lgv_size)
{
    lgv_method_set_add_new(set, "~unref", NULL, NULL, lgv_size__unref);
    lgv_method_set_add_new(set, "+", NULL, NULL, lgv_size__add);
}

int
lgv_ref_new_size(struct lgv_ref *ref, size_t initial_value)
{
    size_t  *self = cork_new(size_t);
    *self = initial_value;
    ref->value = self;
    ref->methods = lgv_simple_method_set_new(lgv_size);
    return 0;
}
