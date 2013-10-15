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


struct s0_interface_type {
    enum s0_interface_type_kind  kind;
    const char  *name;
    s0_type_var  var;
    struct cork_hash_table  *methods;
};


static struct s0_method *
s0_method_new(const char *name, struct s0_formals *params)
{
    struct s0_method  *method = cork_new(struct s0_method);
    method->name = cork_strdup(name);
    method->params = params;
    return method;
}

static void
s0_method_free(struct s0_method *method)
{
    cork_strfree(method->name);
    s0_formals_free(method->params);
    free(method);
}


static enum cork_hash_table_map_result
free_method(struct cork_hash_table_entry *entry, void *user_data)
{
    struct s0_method  *method = entry->value;
    s0_method_free(method);
    return CORK_HASH_TABLE_MAP_DELETE;
}

void
s0_interface_type_free(struct s0_interface_type *itype)
{
    switch (itype->kind) {
        case S0_INTERFACE_TYPE_VAR:
            break;

        case S0_INTERFACE_TYPE_METHOD_SET:
            cork_hash_table_map(itype->methods, free_method, NULL);
            cork_hash_table_free(itype->methods);
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
    itype->name = cork_strdup(name);
    itype->kind = S0_INTERFACE_TYPE_METHOD_SET;
    itype->methods = cork_string_hash_table_new(0);
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
        struct s0_method  *method = s0_method_new(method_name, params);
        entry->key = (void *) method->name;
        entry->value = method;
        return 0;
    } else {
        s0_formals_free(params);
        s0_redefined
            ("Method named %s in %s already exists",
             method_name, itype->name);
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
        s0_undefined("No method named %s in %s", method_name, itype->name);
    }
    return method;
}
