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


struct s0_method_priv {
    struct s0_method  public;
    struct cork_dllist_item  item;
};

struct s0_interface_type {
    enum s0_interface_type_kind  kind;
    const char  *name;
    s0_type_var  var;
    struct cork_hash_table  *methods;
    struct cork_dllist  method_list;
};


static struct s0_method_priv *
s0_method_new(const char *name, struct s0_formals *params)
{
    struct s0_method_priv  *method = cork_new(struct s0_method_priv);
    method->public.name = cork_strdup(name);
    method->public.params = params;
    return method;
}

static void
s0_method_free(struct s0_method_priv *method)
{
    cork_strfree(method->public.name);
    s0_formals_free(method->public.params);
    free(method);
}

static void
s0_method_to_string(struct s0_method_priv *method,
                    struct cork_buffer *dest, size_t indent)
{
    s0_indent(dest, indent);
    cork_buffer_append_printf(dest, "%s {\n", method->public.name);
    s0_formals_to_string(method->public.params, dest, indent + 2);
    s0_indent(dest, indent);
    cork_buffer_append_string(dest, "}\n");
}


static void
free_method(struct cork_dllist_item *item, void *user_data)
{
    struct s0_method_priv  *method =
        cork_container_of(item, struct s0_method_priv, item);
    cork_dllist_remove(item);
    s0_method_free(method);
}

void
s0_interface_type_free(struct s0_interface_type *itype)
{
    switch (itype->kind) {
        case S0_INTERFACE_TYPE_VAR:
            break;

        case S0_INTERFACE_TYPE_METHOD_SET:
            if (itype->name != NULL) {
                cork_strfree(itype->name);
            }
            cork_hash_table_free(itype->methods);
            cork_dllist_map(&itype->method_list, free_method, NULL);
            break;

        default:
            cork_unreachable();
    }
    free(itype);
}

enum s0_interface_type_kind
s0_interface_type_kind(const struct s0_interface_type *itype)
{
    return itype->kind;
}

void
s0_interface_type_to_string(const struct s0_interface_type *itype,
                            struct cork_buffer *dest, size_t indent)
{
    switch (itype->kind) {
        case S0_INTERFACE_TYPE_VAR:
            cork_buffer_append_printf(dest, "i%u", itype->var);
            break;

        case S0_INTERFACE_TYPE_METHOD_SET:
        {
            struct cork_dllist_item  *curr;
            if (itype->name == NULL) {
                cork_buffer_append_string(dest, "interface {\n");
            } else {
                cork_buffer_append_printf
                    (dest, "interface %s {\n", itype->name);
            }
            for (curr = cork_dllist_start(&itype->method_list);
                 !cork_dllist_is_end(&itype->method_list, curr);
                 curr = curr->next) {
                struct s0_method_priv  *method =
                    cork_container_of(curr, struct s0_method_priv, item);
                s0_method_to_string(method, dest, indent + 2);
            }
            s0_indent(dest, indent);
            cork_buffer_append_string(dest, "}\n");
            break;
        }

        default:
            cork_unreachable();
    }
}


struct s0_interface_type *
s0_interface_type_new_var(s0_type_var var)
{
    struct s0_interface_type  *itype = cork_new(struct s0_interface_type);
    itype->name = "var";  // TODO: Render the variable as a string
    itype->kind = S0_INTERFACE_TYPE_VAR;
    itype->var = var;
    return itype;
}

s0_type_var
s0_interface_type_var(const struct s0_interface_type *itype)
{
    return itype->var;
}


struct s0_interface_type *
s0_interface_type_new_method_set(const char *name)
{
    struct s0_interface_type  *itype = cork_new(struct s0_interface_type);
    if (name == NULL) {
        itype->name = NULL;
    } else {
        itype->name = cork_strdup(name);
    }
    itype->kind = S0_INTERFACE_TYPE_METHOD_SET;
    itype->methods = cork_string_hash_table_new(0);
    cork_dllist_init(&itype->method_list);
    return itype;
}

int
s0_interface_type_add_method(struct s0_interface_type *itype,
                             const char *method_name,
                             struct s0_formals *params)
{
    bool  is_new;
    struct cork_hash_table_entry  *entry;
    entry = cork_hash_table_get_or_create
        (itype->methods, (void *) method_name, &is_new);
    if (is_new) {
        struct s0_method_priv  *method = s0_method_new(method_name, params);
        entry->key = (void *) method->public.name;
        entry->value = method;
        cork_dllist_add(&itype->method_list, &method->item);
        return 0;
    } else {
        s0_formals_free(params);
        s0_redefined
            ("Method named %s in %s already exists",
             method_name, itype->name == NULL? "interface": itype->name);
        return -1;
    }
}

const struct s0_method *
s0_interface_type_get_method(struct s0_interface_type *itype,
                             const char *method_name)
{
    return cork_hash_table_get(itype->methods, (void *) method_name);
}

const struct s0_method *
s0_interface_type_require_method(struct s0_interface_type *itype,
                                 const char *method_name)
{
    struct s0_method  *method =
        cork_hash_table_get(itype->methods, (void *) method_name);
    if (CORK_UNLIKELY(method == NULL)) {
        s0_undefined
            ("No method named %s in %s",
             method_name, itype->name == NULL? "interface": itype->name);
    }
    return method;
}
