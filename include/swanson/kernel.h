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

bool
swan_value_is_opset(struct swan_value *value);

int
swan_opset_value(struct swan_value *dest, struct swan_opset *opset);

CORK_ATTR_UNUSED
static inline struct swan_opset *
swan_value_to_opset(struct swan_value *value)
{
    if (CORK_UNLIKELY(value->content == NULL)) {
        swan_bad_value("Expected an opset, got an empty value");
        return NULL;
    } else if (CORK_UNLIKELY(!swan_value_is_opset(value))) {
        swan_bad_value("Expected an opset");
        return NULL;
    } else {
        return value->content;
    }
}


/*-----------------------------------------------------------------------
 * Types
 */

struct swan_type {
    size_t  instance_size;
    struct swan_operation  *init;
    struct swan_operation  *get;
    struct swan_operation  *set;
};

bool
swan_value_is_type(struct swan_value *value);

int
swan_type_new(struct swan_value *dest, size_t instance_size,
              struct swan_operation *init, struct swan_operation *get,
              struct swan_operation *set);

CORK_ATTR_UNUSED
static inline struct swan_type *
swan_value_to_type(struct swan_value *value)
{
    if (CORK_UNLIKELY(value->content == NULL)) {
        swan_bad_value("Expected an type, got an empty value");
        return NULL;
    } else if (CORK_UNLIKELY(!swan_value_is_type(value))) {
        swan_bad_value("Expected an type");
        return NULL;
    } else {
        return value->content;
    }
}


/*-----------------------------------------------------------------------
 * Assignables
 */

struct swan_assignable {
    void  *content;
    struct swan_operation  *get;
    struct swan_operation  *set;
    struct swan_operation  *unref;
};

bool
swan_value_is_assignable(struct swan_value *value);

CORK_ATTR_UNUSED
static inline struct swan_assignable *
swan_value_to_assignable(struct swan_value *value)
{
    if (CORK_UNLIKELY(value->content == NULL)) {
        swan_bad_value("Expected an assignable, got an empty value");
        return NULL;
    } else if (CORK_UNLIKELY(!swan_value_is_assignable(value))) {
        swan_bad_value("Expected an assignable");
        return NULL;
    } else {
        return value->content;
    }
}

int
swan_assignable_alloc(struct swan_value *dest, struct swan_type *type);

int
swan_assignable_value(struct swan_value *dest, void *content,
                      struct swan_operation *get, struct swan_operation *set,
                      struct swan_operation *unref);


/*-----------------------------------------------------------------------
 * Arrays
 */

bool
swan_value_is_fixed_array(struct swan_value *value);

CORK_ATTR_UNUSED
static inline void *
swan_value_to_fixed_array(struct swan_value *value)
{
    if (CORK_UNLIKELY(value->content == NULL)) {
        swan_bad_value("Expected a fixed-length array, got an empty value");
        return NULL;
    } else if (CORK_UNLIKELY(!swan_value_is_fixed_array(value))) {
        swan_bad_value("Expected a fixed-length array");
        return NULL;
    } else {
        return value->content;
    }
}

int
swan_fixed_array_alloc(struct swan_value *dest, struct swan_type *type,
                       size_t element_count);

int
swan_fixed_array_value(struct swan_value *dest, void *elements,
                       size_t element_size, size_t element_count,
                       struct swan_operation *get, struct swan_operation *set);


/*-----------------------------------------------------------------------
 * Built-in types
 */

/* size */

bool
swan_value_is_size(struct swan_value *value);

int
swan_size_new(struct swan_value *dest, size_t value);

CORK_ATTR_UNUSED
static inline size_t *
swan_value_to_size(struct swan_value *value)
{
    if (CORK_UNLIKELY(value->content == NULL)) {
        swan_bad_value("Expected a size, got an empty value");
        return NULL;
    } else if (CORK_UNLIKELY(!swan_value_is_size(value))) {
        swan_bad_value("Expected a size");
        return NULL;
    } else {
        return value->content;
    }
}

/* string */

struct swan_string {
    void  *data;
    size_t  size;
};

#define swan_value_is_string(value)  ((value)->opset != NULL)

CORK_ATTR_UNUSED
static inline struct swan_string *
swan_value_to_string(struct swan_value *value)
{
    if (CORK_UNLIKELY(value->content == NULL)) {
        swan_bad_value("Expected a string, got an empty value");
    }
    return value->content;
}

/* Makes a copy of content */
void
swan_string_new(struct swan_value *dest, const void *data, size_t size);


#endif /* SWANSON_KERNEL_H */
