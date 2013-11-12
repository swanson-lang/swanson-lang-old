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


struct s0_formal_priv {
    struct s0_formal  public;
    struct cork_dllist_item  item;
};

struct s0_formals {
    const char  *name;
    struct cork_hash_table  *params;
    struct cork_dllist  param_list;
};


static struct s0_formal_priv *
s0_formal_new(const char *name,
              struct s0_ref_type *pre, struct s0_ref_type *post)
{
    struct s0_formal_priv  *formal = cork_new(struct s0_formal_priv);
    formal->public.name = cork_strdup(name);
    formal->public.pre = pre;
    formal->public.post = post;
    return formal;
}

static void
s0_formal_free(struct s0_formal_priv *formal)
{
    cork_strfree(formal->public.name);
    s0_ref_type_free(formal->public.pre);
    s0_ref_type_free(formal->public.post);
    free(formal);
}


struct s0_formals *
s0_formals_new(const char *name)
{
    struct s0_formals  *formals = cork_new(struct s0_formals);
    formals->name = cork_strdup(name);
    formals->params = cork_string_hash_table_new(0);
    cork_dllist_init(&formals->param_list);
    return formals;
}

static void
free_param(struct cork_dllist_item *item, void *user_data)
{
    struct s0_formal_priv  *param =
        cork_container_of(item, struct s0_formal_priv, item);
    cork_dllist_remove(item);
    s0_formal_free(param);
}

void
s0_formals_free(struct s0_formals *formals)
{
    cork_hash_table_free(formals->params);
    cork_dllist_map(&formals->param_list, free_param, NULL);
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
        struct s0_formal_priv  *formal = s0_formal_new(param_name, pre, post);
        entry->key = (void *) formal->public.name;
        entry->value = formal;
        cork_dllist_add(&formals->param_list, &formal->item);
        return 0;
    } else {
        s0_ref_type_free(pre);
        s0_ref_type_free(post);
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
    struct cork_dllist_item  *curr;
    for (curr = cork_dllist_start(&formals->param_list);
         !cork_dllist_is_end(&formals->param_list, curr); curr = curr->next) {
        struct s0_formal_priv  *formal =
            cork_container_of(curr, struct s0_formal_priv, item);
        rii_check(map(user_data, &formal->public));
    }
    return 0;
}

void
s0_formals_to_string(const struct s0_formals *formals,
                     struct cork_buffer *dest, size_t indent)
{
    struct cork_dllist_item  *curr;
    for (curr = cork_dllist_start(&formals->param_list);
         !cork_dllist_is_end(&formals->param_list, curr); curr = curr->next) {
        struct s0_formal_priv  *formal =
            cork_container_of(curr, struct s0_formal_priv, item);
        s0_indent(dest, indent);
        cork_buffer_append_printf(dest, "%s: ", formal->public.name);
        s0_ref_type_to_string(formal->public.pre, dest, indent);
        cork_buffer_append_string(dest, " -> ");
        s0_ref_type_to_string(formal->public.post, dest, indent);
        cork_buffer_append_string(dest, "\n");
    }
}
