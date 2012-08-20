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

#include "swanson/metamodel.h"
#include "swanson/s0.h"


struct swan_scope_entry {
    struct swan_value  value;
    bool  block_must_unref;
};

static bool
string_comparator(const void *vk1, const void *vk2)
{
    const char  *k1 = vk1;
    const char  *k2 = vk2;
    return strcmp(k1, k2) == 0;
}

static cork_hash
string_hasher(const void *vk)
{
    const char  *k = vk;
    size_t  len = strlen(k);
    return cork_hash_buffer(0, k, len);
}

struct swan_scope *
swan_scope_new(const char *name)
{
    struct swan_scope  *self = cork_new(struct swan_scope);
    self->name = cork_strdup(name);
    cork_hash_table_init(&self->entries, 0, string_hasher, string_comparator);
    return self;
}

int
swan_scope_check_values(struct swan_scope *self)
{
    struct cork_hash_table_iterator  iter;
    struct cork_hash_table_entry  *entry;
    cork_hash_table_iterator_init(&self->entries, &iter);
    while ((entry = cork_hash_table_iterator_next(&iter)) != NULL) {
        struct swan_scope_entry  *scope_entry = entry->value;
        if (scope_entry->block_must_unref &&
            !swan_value_is_empty(&scope_entry->value)) {
            swan_dangling_reference("%s not unreferenced", (char *) entry->key);
            return -1;
        }
    }
    return 0;
}

void
swan_scope_free(struct swan_scope *self)
{
    struct cork_hash_table_iterator  iter;
    struct cork_hash_table_entry  *entry;
    cork_hash_table_iterator_init(&self->entries, &iter);
    while ((entry = cork_hash_table_iterator_next(&iter)) != NULL) {
        struct swan_scope_entry  *scope_entry = entry->value;
        cork_strfree((const char *) entry->key);
        free(scope_entry);
    }

    cork_hash_table_done(&self->entries);
    cork_strfree(self->name);
    free(self);
}

static void
swan_scope_add_raw(struct swan_scope *self, const char *name,
                   struct swan_value *value, bool block_must_unref)
{
    bool  is_new;
    struct swan_scope_entry  *scope_entry;
    struct cork_hash_table_entry  *entry =
        cork_hash_table_get_or_create
        (&self->entries, (void *) name, &is_new);

    if (is_new) {
        /* If this is a new entry, we need to make a copy of the identifier and
         * allocate a new swan_value to store in the hash table. */
        scope_entry = cork_new(struct swan_scope_entry);
        entry->key = (void *) cork_strdup(name);
        entry->value = scope_entry;
        scope_entry->block_must_unref = block_must_unref;
    } else {
        /* Otherwise we can keep the existing identifier (which we'll have
         * copied in a previous call).  We *don't* unref the value, because
         * that's the responsibility of the operation that does the update. */
        scope_entry = entry->value;
    }

    /* We steal a reference to value. */
    scope_entry->value = *value;
}

void
swan_scope_add(struct swan_scope *self, const char *name,
               struct swan_value *value)
{
    swan_scope_add_raw(self, name, value, true);
}

void
swan_scope_add_predefined(struct swan_scope *self, const char *name,
                          struct swan_value *value)
{
    swan_scope_add_raw(self, name, value, false);
}

void
swan_scope_get(struct swan_scope *self, const char *name,
               struct swan_value *dest)
{
    struct swan_value  *value = cork_hash_table_get(&self->entries, name);
    if (value == NULL) {
        swan_value_clear(dest);
    } else {
        *dest = *value;
    }
}
