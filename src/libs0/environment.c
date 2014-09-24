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


struct s0_environment_key {
    size_t  key_size;
    const void  *key;
};

static struct s0_environment_key *
s0_environment_key_new(size_t key_size, const void *key)
{
    struct s0_environment_key  *k =
        cork_malloc(sizeof(struct s0_environment_key) + key_size);
    void  *key_copy = (void *) (k + 1);
    memcpy(key_copy, key, key_size);
    k->key_size = key_size;
    k->key = key_copy;
    return k;
}

static void
s0_environment_key__free(void *vk)
{
    struct s0_environment_key  *k = vk;
    free(k);
}

static bool
s0_environment_key__equals(void *user_data, const void *vk1, const void *vk2)
{
    const struct s0_environment_key  *k1 = vk1;
    const struct s0_environment_key  *k2 = vk2;
    if (k1->key_size != k2->key_size) {
        return false;
    } else {
        return memcmp(k1->key, k2->key, k1->key_size) == 0;
    }
}

static cork_hash
s0_environment_key__hash(void *user_data, const void *vk)
{
    const struct s0_environment_key  *k = vk;
    return cork_hash_buffer
        (0xff602101 /* "s0_environment" */, k->key, k->key_size);
}


static void
s0_environment_value__free(void *vv)
{
    struct s0_thing  *v = vv;
    s0_thing_free(v);
}


struct s0_environment {
    struct cork_hash_table  *entries;
};

struct s0_environment *
s0_environment_new(void)
{
    struct s0_environment  *environment = cork_new(struct s0_environment);
    environment->entries = cork_hash_table_new(0, 0);
    cork_hash_table_set_free_key
        (environment->entries, s0_environment_key__free);
    cork_hash_table_set_free_value
        (environment->entries, s0_environment_value__free);
    cork_hash_table_set_equals
        (environment->entries, s0_environment_key__equals);
    cork_hash_table_set_hash
        (environment->entries, s0_environment_key__hash);
    return environment;
}

void
s0_environment_free(struct s0_environment *environment)
{
    cork_hash_table_free(environment->entries);
    free(environment);
}

struct s0_thing *
s0_environment_get(struct s0_environment *environment,
                   size_t key_size, const void *key)
{
    struct s0_environment_key  search_key;
    search_key.key_size = key_size;
    search_key.key = key;
    return cork_hash_table_get(environment->entries, &search_key);
}

int
s0_environment_put(struct s0_environment *environment,
                   size_t key_size, const void *key, struct s0_thing *value)
{
    bool  is_new;
    struct s0_environment_key  search_key;
    struct cork_hash_table_entry  *entry;

    search_key.key_size = key_size;
    search_key.key = key;
    is_new = cork_hash_table_get_or_create
        (environment->entries, &search_key, &is_new);

    if (is_new) {
        struct s0_environment_key  *real_key =
            s0_environment_key_new(key_size, key);
        entry->key = real_key;
        entry->value = value;
        return true;
    } else {
        struct s0_thing  *old_value = entry->value;
        s0_thing_free(old_value);
        entry->value = value;
        return false;
    }
}
