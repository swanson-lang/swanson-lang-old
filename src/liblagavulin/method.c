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


struct lgv_method {
    void  *user_data;
    cork_free_f  free_user_data;
    lgv_method_invoke_f  invoke;
};


struct lgv_method *
lgv_method_new(void *user_data, cork_free_f free_user_data,
               lgv_method_invoke_f invoke)
{
    struct lgv_method  *method = cork_new(struct lgv_method);
    method->user_data = user_data;
    method->free_user_data = free_user_data;
    method->invoke = invoke;
    return method;
}

void
lgv_method_free(struct lgv_method *method)
{
    cork_free_user_data(method);
    free(method);
}

int
lgv_method_invoke(struct lgv_method *method, size_t param_count,
                  struct lgv_parameter *params)
{
    return method->invoke(method->user_data, param_count, params);
}


struct lgv_ref *
lgv_parameter_get_ref(const char *name, size_t param_count,
                      struct lgv_parameter *params)
{
    size_t  i;
    for (i = 0; i < param_count; i++) {
        if (strcmp(name, params[i].name) == 0) {
            return params[i].ref;
        }
    }
    lgv_bad_method_call("Expected a parameter named %s", name);
    return NULL;
}

void *
lgv_parameter_get_value(const char *name, size_t param_count,
                        struct lgv_parameter *params)
{
    struct lgv_ref  *ref;
    rpp_check(ref = lgv_parameter_get_ref(name, param_count, params));
    if (ref == NULL) {
        return ref;
    } else {
        return ref->value;
    }
}
