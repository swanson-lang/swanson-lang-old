/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>

#include "libcork/core/allocator.h"
#include "libcork/core/hash.h"
#include "libcork/ds/hash-table.h"
#include "s0.h"


/*-----------------------------------------------------------------------
 * Block specs
 */

struct s0_block_spec_key {
    size_t  key_size;
    const void  *key;
    struct s0_block_spec_entry  value;
};

static struct s0_block_spec_key *
s0_block_spec_key_new(size_t key_size, const void *key,
                      struct s0_type *input_type, struct s0_type *output_type)
{
    struct s0_block_spec_key  *e =
        cork_malloc(sizeof(struct s0_block_spec_key) + key_size);
    void  *key_copy = (void *) (e + 1);
    memcpy(key_copy, key, key_size);
    e->key_size = key_size;
    e->key = key_copy;
    e->value.input_type = input_type;
    e->value.output_type = output_type;
    return e;
}

static void
s0_block_spec_key__free(void *ve)
{
    struct s0_block_spec_key  *e = ve;
    s0_type_free(e->value.input_type);
    s0_type_free(e->value.output_type);
    free(e);
}

static bool
s0_block_spec_key__equals(void *user_data, const void *ve1, const void *ve2)
{
    const struct s0_block_spec_key  *e1 = ve1;
    const struct s0_block_spec_key  *e2 = ve2;
    if (e1->key_size != e2->key_size) {
        return false;
    } else {
        return memcmp(e1->key, e2->key, e1->key_size) == 0;
    }
}

static cork_hash
s0_block_spec_key__hash(void *user_data, const void *ve)
{
    const struct s0_block_spec_key  *e = ve;
    return cork_hash_buffer
        (0x0cfe7f5f /* "s0_block_spec" */, e->key, e->key_size);
}


struct s0_block_spec {
    struct cork_hash_table  *entries;
};

struct s0_block_spec *
s0_block_spec_new(void)
{
    struct s0_block_spec  *block_spec = cork_new(struct s0_block_spec);
    block_spec->entries = cork_hash_table_new(0, 0);
    cork_hash_table_set_free_key
        (block_spec->entries, s0_block_spec_key__free);
    cork_hash_table_set_equals
        (block_spec->entries, s0_block_spec_key__equals);
    cork_hash_table_set_hash
        (block_spec->entries, s0_block_spec_key__hash);
    return block_spec;
}

void
s0_block_spec_free(struct s0_block_spec *block_spec)
{
    cork_hash_table_free(block_spec->entries);
    free(block_spec);
}

struct s0_block_spec_entry *
s0_block_spec_get(struct s0_block_spec *block_spec,
                  size_t key_size, const void *key)
{
    struct s0_block_spec_key  search_key;
    struct cork_hash_table_entry  *entry;
    search_key.key_size = key_size;
    search_key.key = key;
    entry = cork_hash_table_get_entry(block_spec->entries, &search_key);
    if (entry == NULL) {
        return NULL;
    } else {
        struct s0_block_spec_key  *result = entry->key;
        return &result->value;
    }
}

int
s0_block_spec_put(struct s0_block_spec *block_spec,
                  size_t key_size, const void *key,
                  struct s0_type *input_type, struct s0_type *output_type)
{
    bool  is_new;
    struct s0_block_spec_key  search_key;
    struct cork_hash_table_entry  *entry;

    search_key.key_size = key_size;
    search_key.key = key;
    is_new = cork_hash_table_get_or_create
        (block_spec->entries, &search_key, &is_new);

    if (is_new) {
        struct s0_block_spec_key  *real_key =
            s0_block_spec_key_new(key_size, key, input_type, output_type);
        entry->key = real_key;
        return true;
    } else {
        struct s0_block_spec_key  *real_key = entry->key;
        s0_type_free(real_key->value.input_type);
        s0_type_free(real_key->value.output_type);
        real_key->value.input_type = input_type;
        real_key->value.output_type = output_type;
        return false;
    }
}

