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


/*-----------------------------------------------------------------------
 * Top-level kernel
 */

void
swan_kernel_get(struct swan_value *dest);


/*-----------------------------------------------------------------------
 * Memory management
 */

CORK_ATTR_PURE
struct swan_opset *
swan_static__opset(void);

struct swan_opset *
swan_explicit_opset_new(struct swan_opset *base);

struct swan_opset *
swan_explicit_opset_new_from_size(struct swan_opset *base,
                                  size_t instance_size);


/*-----------------------------------------------------------------------
 * Built-in types
 */

/* size */

CORK_ATTR_PURE
struct swan_opset *
swan_size__static__opset(void);

CORK_ATTR_PURE
struct swan_opset *
swan_size__explicit__opset(void);

void
swan_size_new(struct swan_value *dest, size_t value);


#endif /* SWANSON_KERNEL_H */
