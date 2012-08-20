/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef SWANSON_S0_H
#define SWANSON_S0_H

#include <libcork/core.h>
#include <libcork/ds.h>

#include <swanson/metamodel.h>


/*-----------------------------------------------------------------------
 * Parsing S0
 */

struct swan_s0_callback {
    int
    (*operation_call)(struct swan_s0_callback *callback,
                      const char *target, const char *operation_name,
                      size_t param_count, const char **params);

    int
    (*string_constant)(struct swan_s0_callback *callback,
                       const char *result, const char *contents,
                       size_t content_length);

    int
    (*start_block)(struct swan_s0_callback *callback, const char *target,
                   size_t param_count, const char **params);

    int
    (*end_block)(struct swan_s0_callback *callback);

    int
    (*finish)(struct swan_s0_callback *callback);
};


int
swan_s0_parse(const char *buf, size_t len, struct swan_s0_callback *callback);


/*-----------------------------------------------------------------------
 * Scope objects
 */

struct swan_scope {
    const char  *name;
    struct cork_hash_table  entries;
};

struct swan_scope *
swan_scope_new(const char *name);

int
swan_scope_check_values(struct swan_scope *scope);

void
swan_scope_free(struct swan_scope *scope);

/* S0 code is responsible for unref-ing value */
void
swan_scope_add(struct swan_scope *scope, const char *name,
               struct swan_value *value);

/* Scope is responsible for unref-ing value */
void
swan_scope_add_predefined(struct swan_scope *scope, const char *name,
                          struct swan_value *value);

void
swan_scope_get(struct swan_scope *self, const char *name,
               struct swan_value *dest);


/*-----------------------------------------------------------------------
 * Block objects
 */

enum swan_statement_type {
    SWAN_STATEMENT_OPERATION_CALL,
    SWAN_STATEMENT_STRING_CONSTANT,
    SWAN_STATEMENT_BLOCK
};

typedef cork_array(const char *)  swan_id_array;

struct swan_block;

struct swan_statement {
    enum swan_statement_type  type;
    const char  *target;
    const char  *operation_name;
    const char  *contents;
    size_t  content_length;
    swan_id_array  params;
    struct swan_block  *block;
    struct cork_dllist_item  list;
};

struct swan_block {
    swan_id_array  params;
    struct cork_dllist  statements;
};

struct swan_statement *
swan_statement_operation_call_new(const char *target, const char *op_name,
                                  size_t param_count, const char **params);

struct swan_statement *
swan_statement_string_constant_new(const char *target, const char *contents,
                                   size_t content_length);

struct swan_statement *
swan_statement_block_new(const char *target, struct swan_block *block);

void
swan_statement_free(struct swan_statement *stmt);

struct swan_block *
swan_block_new(size_t param_count, const char **params);

struct swan_block *
swan_kernel_block_new(void);

/* Blocks are garbage-collected; use cork_gc_decref to free them. */

#define swan_block_add_statement(b, s) \
    cork_dllist_add(&(b)->statements, &(s)->list)


int
swan_s0_parse_block(struct swan_block *dest, const char *buf, size_t len);


/*-----------------------------------------------------------------------
 * Evaluating S0
 */

int
swan_block_evaluate(struct swan_block *block, const char *scope_name,
                    size_t param_count, struct swan_value *params);

int
swan_kernel_block_evaluate(struct swan_block *block);


#endif /* SWANSON_S0_H */
