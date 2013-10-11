/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <libcork/core.h>
#include <libcork/ds.h>
#include <libcork/helpers/errors.h>

#include "s0.h"


struct s0_formals {
    const char  *name;
    struct cork_hash_table  *params;
};


static struct s0_formal *
s0_formal_new(const char *name,
              struct s0_ref_type *pre, struct s0_ref_type *post)
{
    struct s0_formal  *formal = cork_new(struct s0_formal);
    formal->name = cork_strdup(name);
    formal->pre = pre;
    formal->post = post;
    return formal;
}

static void
s0_formal_free(struct s0_formal *formal)
{
    cork_strfree(formal->name);
    // TODO: s0_ref_type_free(formal->pre);
    // TODO: s0_ref_type_free(formal->post);
    free(formal);
}


struct s0_formals *
s0_formals_new(const char *name)
{
    struct s0_formals  *formals = cork_new(struct s0_formals);
    formals->name = cork_strdup(name);
    formals->params = cork_string_hash_table_new(0);
    return formals;
}

static enum cork_hash_table_map_result
free_param(struct cork_hash_table_entry *entry, void *user_data)
{
    struct s0_formal  *param = entry->value;
    s0_formal_free(param);
    return CORK_HASH_TABLE_MAP_DELETE;
}

void
s0_formals_free(struct s0_formals *formals)
{
    cork_hash_table_map(formals->params, free_param, NULL);
    cork_hash_table_free(formals->params);
    cork_strfree(formals->name);
    free(formals);
}

int
s0_formals_add(struct s0_formals *formals, const char *param_name,
               struct s0_ref_type *pre, struct s0_ref_type *post)
{
    bool  is_new;
    struct cork_hash_table_entry  *entry;
    entry = cork_hash_table_get_or_create
        (formals->params, (void *) param_name, &is_new);
    if (is_new) {
        struct s0_formal  *formal = s0_formal_new(param_name, pre, post);
        entry->key = (void *) formal->name;
        entry->value = formal;
        return 0;
    } else {
        // TODO: s0_ref_type_free(pre);
        // TODO: s0_ref_type_free(post);
        s0_redefined
            ("Parameter named %s in %s already exists",
             param_name, formals->name);
        return -1;
    }
}

const struct s0_formal *
s0_formals_get(const struct s0_formals *formals, const char *param_name)
{
    return cork_hash_table_get(formals->params, (void *) param_name);
}

const struct s0_formal *
s0_formals_require(const struct s0_formals *formals, const char *param_name)
{
    struct s0_formal  *formal =
        cork_hash_table_get(formals->params, (void *) param_name);
    if (CORK_UNLIKELY(formal == NULL)) {
        s0_undefined("No parameter named %s in %s", param_name, formals->name);
    }
    return formal;
}

int
s0_formals_map(const struct s0_formals *formals, s0_formal_map_f map,
               void *user_data)
{
    struct cork_hash_table_iterator  iter;
    struct cork_hash_table_entry  *entry;
    cork_hash_table_iterator_init(formals->params, &iter);
    while ((entry = cork_hash_table_iterator_next(&iter)) != NULL) {
        struct s0_formal  *formal = entry->value;
        rii_check(map(user_data, formal));
    }
    return 0;
}
