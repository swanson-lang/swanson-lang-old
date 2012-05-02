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

void
swan_scope_free(struct swan_scope *scope);

void
swan_scope_add(struct swan_scope *scope, const char *name,
               struct swan_value *value);

void
swan_scope_get(struct swan_scope *self, const char *name,
               struct swan_value *dest);


/*-----------------------------------------------------------------------
 * Evaluating S0
 */

struct swan_s0_evaluator {
    struct swan_s0_callback  callback;
    struct swan_scope  *scope;
};

struct swan_s0_evaluator *
swan_s0_evaluator_new_empty(const char *scope_name);

struct swan_s0_evaluator *
swan_s0_evaluator_new_kernel(void);

void
swan_s0_evaluator_free(struct swan_s0_evaluator *eval);


#endif /* SWANSON_S0_H */
