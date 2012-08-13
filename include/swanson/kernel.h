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

/* hash of "opset" */
#define SWAN_REP_OPSET  0x1ccfa032
#define swan_value_is_opset(value)  swan_value_is(value, SWAN_REP_OPSET)
swan_value_define_to(opset, struct swan_opset, SWAN_REP_OPSET,
                     "Expected an opset");

int
swan_opset_value(struct swan_value *dest, struct swan_opset *opset);


/*-----------------------------------------------------------------------
 * Types
 */

struct swan_type {
    size_t  instance_size;
    struct swan_opset  *opset;
};

/* hash of "type" */
#define SWAN_REP_TYPE  0x79f23513
#define swan_value_is_type(value)  swan_value_is(value, SWAN_REP_TYPE)
swan_value_define_to(type, struct swan_type, SWAN_REP_TYPE,
                     "Expected an type");

int
swan_type_new(struct swan_value *dest, size_t instance_size,
              struct swan_opset *opset);


/*-----------------------------------------------------------------------
 * Assignables
 */

struct swan_allocator {
    size_t  instance_size;
    struct swan_opset  *base_opset;
    struct swan_opset  assignable_opset;
};

struct swan_assignable {
    void  *content;
};

/* hash of "allocator" */
#define SWAN_REP_ALLOCATOR  0xeb36369d
#define swan_value_is_allocator(value) \
    swan_value_is(value, SWAN_REP_ALLOCATOR)
swan_value_define_to(allocator, struct swan_allocator, SWAN_REP_ALLOCATOR,
                     "Expected an allocator");

int
swan_allocator_new(struct swan_value *dest, struct swan_type *type);

/* hash of "assignable" */
#define SWAN_REP_ASSIGNABLE  0x0666f40c
#define swan_value_is_assignable(value) \
    swan_value_is(value, SWAN_REP_ASSIGNABLE)
swan_value_define_to(assignable, struct swan_assignable, SWAN_REP_ASSIGNABLE,
                     "Expected an assignable");


/*-----------------------------------------------------------------------
 * Arrays
 */

int
swan_array_type_new(struct swan_value *dest, struct swan_type *element_type,
                    size_t element_count);


/*-----------------------------------------------------------------------
 * Sizes
 */

/* hash of "size" */
#define SWAN_REP_SIZE  0xed8917fa
#define swan_value_is_size(value)  swan_value_is(value, SWAN_REP_SIZE)
swan_value_define_to(size, size_t, SWAN_REP_SIZE,
                     "Expected an size");

int
swan_size_new(struct swan_value *dest, size_t value);


/*-----------------------------------------------------------------------
 * Strings
 */

struct swan_string {
    void  *data;
    size_t  size;
};

/* hash of "string" */
#define SWAN_REP_STRING  0xad217aab
#define swan_value_is_string(value)  swan_value_is(value, SWAN_REP_STRING)
swan_value_define_to(string, struct swan_string, SWAN_REP_STRING,
                     "Expected an string");

/* Makes a copy of content */
int
swan_string_new(struct swan_value *dest, const void *data, size_t size);


#endif /* SWANSON_KERNEL_H */
