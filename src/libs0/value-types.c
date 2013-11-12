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


struct s0_value_type {
    enum s0_value_type_kind  kind;
    s0_type_var  var;
};

void
s0_value_type_free(struct s0_value_type *vtype)
{
    free(vtype);
}

enum s0_value_type_kind
s0_value_type_kind(const struct s0_value_type *vtype)
{
    return vtype->kind;
}

void
s0_value_type_to_string(const struct s0_value_type *vtype,
                        struct cork_buffer *dest, size_t indent)
{
    switch (vtype->kind) {
        case S0_VALUE_TYPE_ANY:
            break;

        case S0_VALUE_TYPE_VAR:
            cork_buffer_append_printf(dest, "[v%u] ", vtype->var);
            break;

        default:
            cork_unreachable();
    }
}


struct s0_value_type *
s0_value_type_new_any(void)
{
    struct s0_value_type  *vtype = cork_new(struct s0_value_type);
    vtype->kind = S0_VALUE_TYPE_ANY;
    return vtype;
}


struct s0_value_type *
s0_value_type_new_var(s0_type_var var)
{
    struct s0_value_type  *vtype = cork_new(struct s0_value_type);
    vtype->kind = S0_VALUE_TYPE_VAR;
    vtype->var = var;
    return vtype;
}

s0_type_var
s0_value_type_var(const struct s0_value_type *vtype)
{
    return vtype->var;
}
