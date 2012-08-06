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
 * Opsets
 */

void
swan_opset_value(struct swan_value *dest, struct swan_opset *opset);


/*-----------------------------------------------------------------------
 * Memory management
 */

int
swan_explicit_allocator_new(struct swan_value *dest, struct swan_value *type);


/*-----------------------------------------------------------------------
 * Built-in types
 */

/* size */

#define swan_value_is_size(value)  ((value)->opset != NULL)

CORK_ATTR_UNUSED
static inline size_t *
swan_value_get_size(struct swan_value *value)
{
    if (CORK_UNLIKELY(value->content == NULL)) {
        swan_bad_value("Expected a size, got an empty value");
    }
    return value->content;
}

/* string */

struct swan_string {
    void  *data;
    size_t  size;
};

/* Makes a copy of content */
void
swan_string_new(struct swan_value *dest, const void *data, size_t size);


#endif /* SWANSON_KERNEL_H */
