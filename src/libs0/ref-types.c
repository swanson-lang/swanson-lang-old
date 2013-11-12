/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <libcork/core.h>
#include <libcork/helpers/errors.h>

#include "s0.h"


struct s0_ref_type {
    struct s0_value_type  *vtype;
    struct s0_interface_type  *itype;
};

struct s0_ref_type *
s0_ref_type_new(struct s0_value_type *vtype, struct s0_interface_type *itype)
{
    struct s0_ref_type  *rtype = cork_new(struct s0_ref_type);
    rtype->vtype = vtype;
    rtype->itype = itype;
    return rtype;
}

void
s0_ref_type_free(struct s0_ref_type *rtype)
{
    s0_value_type_free(rtype->vtype);
    s0_interface_type_free(rtype->itype);
    free(rtype);
}

const struct s0_value_type *
s0_ref_type_value_type(const struct s0_ref_type *rtype)
{
    return rtype->vtype;
}

const struct s0_interface_type *
s0_ref_type_interface_type(const struct s0_ref_type *rtype)
{
    return rtype->itype;
}

void
s0_ref_type_to_string(const struct s0_ref_type *rtype,
                      struct cork_buffer *dest, size_t indent)
{
    s0_value_type_to_string(rtype->vtype, dest, indent);
    s0_interface_type_to_string(rtype->itype, dest, indent);
}
