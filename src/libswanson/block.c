/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <string.h>

#include <libcork/core.h>
#include <libcork/ds.h>
#include <libcork/helpers/errors.h>
#include <libcork/helpers/gc.h>

#include "swanson/metamodel.h"
#include "swanson/s0.h"


/*-----------------------------------------------------------------------
 * Blocks
 */

struct swan_statement *
swan_statement_operation_call_new(const char *target, const char *operation_name,
                                  size_t param_count, const char **params)
{
    size_t  i;
    struct swan_statement  *self = cork_new(struct swan_statement);
    self->type = SWAN_STATEMENT_OPERATION_CALL;
    self->target = cork_strdup(target);
    self->operation_name = cork_strdup(operation_name);
    self->contents = NULL;
    cork_array_init(&self->params);
    for (i = 0; i < param_count; i++) {
        cork_array_append(&self->params, cork_strdup(params[i]));
    }
    self->block = NULL;
    return self;
}

struct swan_statement *
swan_statement_string_constant_new(const char *target, const char *contents,
                                   size_t content_length)
{
    struct swan_statement  *self = cork_new(struct swan_statement);
    self->type = SWAN_STATEMENT_STRING_CONSTANT;
    self->target = cork_strdup(target);
    self->operation_name = NULL;
    self->contents = cork_malloc(content_length);
    memcpy((char *) self->contents, contents, content_length);
    self->content_length = content_length;
    cork_array_init(&self->params);
    self->block = NULL;
    return self;
}

struct swan_statement *
swan_statement_block_new(const char *target, struct swan_block *block)
{
    struct swan_statement  *self = cork_new(struct swan_statement);
    self->type = SWAN_STATEMENT_BLOCK;
    self->target = cork_strdup(target);
    self->operation_name = NULL;
    self->contents = NULL;
    cork_array_init(&self->params);
    self->block = block;
    return self;
}

void
swan_statement_free(struct swan_statement *self)
{
    size_t  i;

    cork_strfree(self->target);
    if (self->operation_name != NULL) {
        cork_strfree(self->operation_name);
    }
    if (self->contents != NULL) {
        free((char *) self->contents);
    }

    for (i = 0; i < cork_array_size(&self->params); i++) {
        cork_strfree(cork_array_at(&self->params, i));
    }
    cork_array_done(&self->params);

    if (self->block != NULL) {
        cork_gc_decref(self->block);
    }

    free(self);
}


_free_(swan_block) {
    struct swan_block  *self = obj;
    struct cork_dllist_item  *curr;
    struct cork_dllist_item  *next;

    for (curr = cork_dllist_start(&self->statements);
         !cork_dllist_is_end(&self->statements, curr); curr = next) {
        struct swan_statement  *stmt =
            cork_container_of(curr, struct swan_statement, list);
        next = curr->next;
        swan_statement_free(stmt);
    }
}

_gc_no_recurse_(swan_block);

struct swan_block *
swan_block_new(size_t param_count, const char **params)
{
    size_t  i;
    struct swan_block  *self = cork_gc_new(swan_block);
    cork_array_init(&self->params);
    for (i = 0; i < param_count; i++) {
        cork_array_append(&self->params, cork_strdup(params[i]));
    }
    cork_dllist_init(&self->statements);
    return self;
}

struct swan_block *
swan_kernel_block_new(void)
{
    static const char  *params = { "kernel" };
    return swan_block_new(1, &params);
}


/*-----------------------------------------------------------------------
 * Block callback
 */

typedef cork_array(struct swan_block *)  block_array;

struct swan_block_callback {
    struct swan_s0_callback  parent;
    struct swan_block  *current_block;
    block_array  block_stack;
};

static struct swan_block *
swan_block_callback_pop(struct swan_block_callback *self)
{
    return cork_array_at
        (&self->block_stack, --cork_array_size(&self->block_stack));
}

static int
swan_block__operation_call(struct swan_s0_callback *vself,
                           const char *target, const char *operation_name,
                           size_t param_count, const char **params)
{
    struct swan_block_callback  *self =
        cork_container_of(vself, struct swan_block_callback, parent);
    struct swan_statement  *stmt = swan_statement_operation_call_new
        (target, operation_name, param_count, params);
    swan_block_add_statement(self->current_block, stmt);
    return 0;
}

static int
swan_block__string_constant(struct swan_s0_callback *vself,
                            const char *result, const char *contents,
                            size_t content_length)
{
    struct swan_block_callback  *self =
        cork_container_of(vself, struct swan_block_callback, parent);
    struct swan_statement  *stmt =
        swan_statement_string_constant_new(result, contents, content_length);
    swan_block_add_statement(self->current_block, stmt);
    return 0;
}

static int
swan_block__start_block(struct swan_s0_callback *vself, const char *target,
                        size_t param_count, const char **params)
{
    struct swan_block_callback  *self =
        cork_container_of(vself, struct swan_block_callback, parent);
    struct swan_block  *new_block = swan_block_new(param_count, params);
    struct swan_statement  *stmt = swan_statement_block_new(target, new_block);
    swan_block_add_statement(self->current_block, stmt);
    cork_array_append(&self->block_stack, self->current_block);
    self->current_block = new_block;
    return 0;
}

static int
swan_block__end_block(struct swan_s0_callback *vself)
{
    struct swan_block_callback  *self =
        cork_container_of(vself, struct swan_block_callback, parent);
    self->current_block = swan_block_callback_pop(self);
    return 0;
}

static int
swan_block__finish(struct swan_s0_callback *vself)
{
    return 0;
}


int
swan_s0_parse_block(struct swan_block *dest, const char *buf, size_t len)
{
    int  rc;
    struct swan_block_callback  callback;
    callback.parent.operation_call = swan_block__operation_call;
    callback.parent.string_constant = swan_block__string_constant;
    callback.parent.start_block = swan_block__start_block;
    callback.parent.end_block = swan_block__end_block;
    callback.parent.finish = swan_block__finish;
    cork_array_init(&callback.block_stack);
    callback.current_block = dest;
    rc = swan_s0_parse(buf, len, &callback.parent);
    cork_array_done(&callback.block_stack);
    return rc;
}
