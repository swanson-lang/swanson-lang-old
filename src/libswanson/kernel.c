/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include "kernel/00-static.c.in"
#include "kernel/size.c.in"

_constant_op_(swan_kernel, sizeof, &swan_kernel_sizeof__opset, NULL);

_extern_opset_(swan_kernel)
{
    get_static_op("sizeof", swan_kernel, sizeof);
    return NULL;
}

void
swan_kernel_get(struct swan_value *dest)
{
    dest->opset = &swan_kernel__opset;
    dest->content = NULL;
}
