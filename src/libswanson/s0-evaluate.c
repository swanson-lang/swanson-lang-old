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

struct swan_s0_evaluator {
    struct swan_scope  *scope;
    value_array  params;
};

static int
swan_s0_evaluate__operation_call(struct swan_s0_evaluator *self,
                                 struct swan_statement *stmt)
{
    size_t  i;
    size_t  param_count;
    struct swan_value  target_value;
    struct swan_operation  *operation;

    DEBUG("Calling %s.%s\n", stmt->target, stmt->operation_content);
    swan_scope_get(self->scope, stmt->target, &target_value);
    if (swan_value_is_empty(&target_value)) {
        swan_s0_undefined_identifier
            ("No value named %s in scope %s", stmt->target, self->scope->name);
        return -1;
    }

    rip_check(operation = swan_value_require_operation
              (&target_value, stmt->operation_name));
    param_count = cork_array_size(&stmt->params);
    cork_array_ensure_size(&self->params, param_count);

    for (i = 0; i < param_count; i++) {
        const char  *param_name = cork_array_at(&stmt->params, i);
        struct swan_value  *param = &cork_array_at(&self->params, i);
        DEBUG("  Param %zu = %s\n", i, param_name);
        swan_scope_get(self->scope, param_name, param);
    }

    rii_check(swan_operation_evaluate
              (operation, param_count, &cork_array_at(&self->params, 0)));

    for (i = 0; i < param_count; i++) {
        const char  *param_name = cork_array_at(&stmt->params, i);
        DEBUG("  Param %zu = %s\n", i, param_name);
        swan_scope_add
            (self->scope, param_name, &cork_array_at(&self->params, i));
    }

    return 0;
}

static int
swan_s0_evaluate__string_constant(struct swan_s0_evaluator *self,
                                  struct swan_statement *stmt)
{
    struct swan_value  str = SWAN_VALUE_EMPTY;
    rii_check(swan_string_new(&str, stmt->contents, stmt->content_length));
    swan_scope_add(self->scope, stmt->target, &str);
    return 0;
}

static int
swan_s0_evaluate__block(struct swan_s0_evaluator *self,
                        struct swan_statement *stmt)
{
    struct swan_value  block = SWAN_VALUE_EMPTY;
    rii_check(swan_block_value(&block, stmt->block));
    swan_scope_add(self->scope, stmt->target, &block);
    return 0;
}


static int
swan_block_evaluate_statement(struct swan_s0_evaluator *self,
                              struct swan_statement *stmt)
{
    switch (stmt->type) {
        case SWAN_STATEMENT_OPERATION_CALL:
            return swan_s0_evaluate__operation_call(self, stmt);

        case SWAN_STATEMENT_STRING_CONSTANT:
            return swan_s0_evaluate__string_constant(self, stmt);

        case SWAN_STATEMENT_BLOCK:
            return swan_s0_evaluate__block(self, stmt);

        default:
            cork_unreachable();
    }
}

static int
swan_block_evaluate_statements(struct swan_s0_evaluator *self,
                               struct swan_block *block)
{
    struct cork_dllist_item  *curr;

    for (curr = cork_dllist_start(&block->statements);
         !cork_dllist_is_end(&block->statements, curr); curr = curr->next) {
        struct swan_statement  *stmt =
            cork_container_of(curr, struct swan_statement, list);
        rii_check(swan_block_evaluate_statement(self, stmt));
    }

    return 0;
}

int
swan_block_evaluate(struct swan_block *block, const char *scope_name,
                    size_t param_count, struct swan_value *params)
{
    size_t  i;
    struct swan_s0_evaluator  self;

    if (param_count != cork_array_size(&block->params)) {
        swan_bad_value
            ("Trying to evaluate block with wrong number of parameters "
             "(got %zu, need %zu)",
             param_count, cork_array_size(&block->params));
        return -1;
    }

    self.scope = swan_scope_new(scope_name);
    cork_array_init(&self.params);

    for (i = 0; i < param_count; i++) {
        const char  *param_name = cork_array_at(&block->params, i);
        swan_scope_add_predefined(self.scope, param_name, &params[i]);
    }

    ei_check(swan_block_evaluate_statements(&self, block));
    ei_check(swan_scope_check_values(self.scope));

    for (i = 0; i < param_count; i++) {
        const char  *param_name = cork_array_at(&block->params, i);
        swan_scope_get(self.scope, param_name, &params[i]);
    }

    cork_array_done(&self.params);
    swan_scope_free(self.scope);
    return 0;

error:
    cork_array_done(&self.params);
    swan_scope_free(self.scope);
    return -1;
}

int
swan_kernel_block_evaluate(struct swan_block *block)
{
    int  rc;
    struct swan_value  kernel = SWAN_VALUE_EMPTY;
    swan_kernel_get(&kernel);
    rc = swan_block_evaluate(block, "globals", 1, &kernel);
    return rc;
}
