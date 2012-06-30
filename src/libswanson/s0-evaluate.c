/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <libcork/core.h>
#include <libcork/ds.h>
#include <libcork/helpers/errors.h>

#include "swanson/kernel.h"
#include "swanson/metamodel.h"
#include "swanson/s0.h"


#if !defined(SWAN_DEBUG_EVALUATOR)
#define SWAN_DEBUG_EVALUATOR  0
#endif

#if SWAN_DEBUG_EVALUATOR
#include <stdio.h>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...) /* no debug messages */
#endif


typedef cork_array(struct swan_value)  value_array;

struct swan_s0_evaluator_internal {
    struct swan_s0_evaluator  public;
    value_array  params;
};

static int
swan_s0_evaluate__operation_call(struct swan_s0_callback *callback,
                                 const char *target, const char *operation_name,
                                 size_t param_count, const char **params)
{
    size_t  i;
    struct swan_s0_evaluator_internal  *eval = cork_container_of
        (callback, struct swan_s0_evaluator_internal, public.callback);
    struct swan_value  target_value;
    struct swan_operation  *operation;

    DEBUG("Calling %s.%s\n", target, operation_name);
    swan_scope_get(eval->public.scope, target, &target_value);
    if (swan_value_is_empty(&target_value)) {
        swan_s0_undefined_identifier
            ("No value named %s in scope %s", target, eval->public.scope->name);
        return -1;
    }

    rip_check(operation =
              swan_value_get_operation(&target_value, operation_name));
    cork_array_ensure_size(&eval->params, param_count);

    for (i = 0; i < param_count; i++) {
        struct swan_value  *param = &cork_array_at(&eval->params, i);
        DEBUG("  Param %zu = %s\n", i, params[i]);
        swan_scope_get(eval->public.scope, params[i], param);
    }

    rii_check(swan_operation_evaluate
              (operation,
               param_count, &cork_array_at(&eval->params, 0)));

    for (i = 0; i < param_count; i++) {
        DEBUG("  Param %zu = %s\n", i, params[i]);
        swan_scope_add
            (eval->public.scope, params[i], &cork_array_at(&eval->params, i));
    }

    return 0;
}

static int
swan_s0_evaluate__string_constant(struct swan_s0_callback *callback,
                                  const char *result, const char *contents,
                                  size_t content_length)
{
    struct swan_s0_evaluator_internal  *eval = cork_container_of
        (callback, struct swan_s0_evaluator_internal, public.callback);
    struct swan_value  str = SWAN_VALUE_EMPTY;
    swan_string_new(&str, contents, content_length);
    swan_scope_add(eval->public.scope, result, &str);
    return 0;
}

static int
swan_s0_evaluate__finish(struct swan_s0_callback *callback)
{
    struct swan_s0_evaluator_internal  *eval = cork_container_of
        (callback, struct swan_s0_evaluator_internal, public.callback);
    return swan_scope_check_values(eval->public.scope);
}

struct swan_s0_evaluator *
swan_s0_evaluator_new_empty(const char *scope_name)
{
    struct swan_s0_evaluator_internal  *self =
        cork_new(struct swan_s0_evaluator_internal);
    self->public.callback.operation_call = swan_s0_evaluate__operation_call;
    self->public.callback.string_constant = swan_s0_evaluate__string_constant;
    self->public.callback.finish = swan_s0_evaluate__finish;
    self->public.scope = swan_scope_new(scope_name);
    cork_array_init(&self->params);
    return &self->public;
}

struct swan_s0_evaluator *
swan_s0_evaluator_new_kernel(void)
{
    struct swan_value  kernel = SWAN_VALUE_EMPTY;
    struct swan_s0_evaluator  *self = swan_s0_evaluator_new_empty("globals");
    swan_kernel_get(&kernel);
    swan_scope_add_predefined(self->scope, "kernel", &kernel);
    return self;
}

void
swan_s0_evaluator_free(struct swan_s0_evaluator *vself)
{
    struct swan_s0_evaluator_internal  *self =
        cork_container_of(vself, struct swan_s0_evaluator_internal, public);
    cork_array_done(&self->params);
    swan_scope_free(self->public.scope);
    free(self);
}
