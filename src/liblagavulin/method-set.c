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

#include "lagavulin.h"


struct lgv_method_set {
    struct cork_hash_table  *methods;
};

struct lgv_method_set *
lgv_method_set_new(void)
{
    struct lgv_method_set  *set = cork_new(struct lgv_method_set);
    set->methods = cork_string_hash_table_new(0);
    return set;
}

static enum cork_hash_table_map_result
lgv_method_set__free_method(struct cork_hash_table_entry *entry, void *dummy)
{
    cork_strfree(entry->key);
    lgv_method_free(entry->value);
    return CORK_HASH_TABLE_MAP_DELETE;
}

void
lgv_method_set_free(struct lgv_method_set *set)
{
    cork_hash_table_map(set->methods, lgv_method_set__free_method, NULL);
    cork_hash_table_free(set->methods);
    free(set);
}

void
lgv_method_set_add(struct lgv_method_set *set, const char *name,
                   struct lgv_method *method)
{
    bool  is_new;
    struct cork_hash_table_entry  *entry;

    entry = cork_hash_table_get_or_create(set->methods, (void *) name, &is_new);
    if (is_new) {
        entry->key = (void *) cork_strdup(name);
    } else {
        lgv_method_free(entry->value);
    }
    entry->value = method;
}

void
lgv_method_set_add_new(struct lgv_method_set *set, const char *name,
                       void *user_data, cork_free_f free_user_data,
                       lgv_method_invoke_f invoke)
{
    struct lgv_method  *method =
        lgv_method_new(user_data, free_user_data, invoke);
    lgv_method_set_add(set, name, method);
}

struct lgv_method *
lgv_method_set_get(struct lgv_method_set *set, const char *name)
{
    return cork_hash_table_get(set->methods, (void *) name);
}

struct lgv_method *
lgv_method_set_require(struct lgv_method_set *set, const char *name,
                       const char *where)
{
    struct lgv_method  *method =
        cork_hash_table_get(set->methods, (void *) name);
    if (CORK_UNLIKELY(method == NULL)) {
        lgv_undefined("No method named %s in %s", name, where);
    }
    return method;
}
