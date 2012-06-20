/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef SWANSON_KERNEL_H
#define SWANSON_KERNEL_H

#include <libcork/core.h>

#include <swanson/metamodel.h>

void
swan_kernel_get(struct swan_value *dest);

extern struct swan_opset  swan_static__opset;

extern struct swan_opset  swan_size__static__opset;

#endif /* SWANSON_KERNEL_H */
