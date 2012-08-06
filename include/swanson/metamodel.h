/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef SWANSON_METAMODEL_H
#define SWANSON_METAMODEL_H

#include <libcork/core.h>
#include <libcork/ds.h>

#include <swanson/error.h>

struct swan_value;


struct swan_operation {
    int
    (*evaluate)(struct swan_operation *op, size_t param_count,
                struct swan_value *params);
};

#define swan_operation_evaluate(op, pc, p) \
    ((op)->evaluate((op), (pc), (p)))


struct swan_opset {
    struct swan_operation *
    (*get_operation)(struct swan_opset *opset, const char *name);

    struct swan_opset *
    (*alias)(struct swan_opset *opset);

    void
    (*unref)(struct swan_opset *opset);
};

#define swan_opset_get_operation(opset, name) \
    ((opset)->get_operation((opset), (name)))

#define swan_opset_alias(opset) \
    ((opset)->alias((opset)))

#define swan_opset_unref(opset) \
    ((opset)->unref((opset)))


struct swan_value {
    void  *content;
    struct swan_opset  *opset;
};

typedef cork_array(struct swan_value)  swan_value_array;

#define SWAN_VALUE_EMPTY  { NULL, NULL }

#define swan_value_clear(value) \
    do { \
        (value)->content = NULL; \
        (value)->opset = NULL; \
    } while (0)

#define swan_value_is_empty(value)  ((value)->opset == NULL)

#define swan_value_get_operation(value, op_name) \
    (swan_opset_get_operation((value)->opset, (op_name)))

CORK_ATTR_UNUSED
static inline struct swan_operation *
swan_value_require_operation(struct swan_value *value, const char *name)
{
    struct swan_operation  *op = swan_value_get_operation(value, name);
    if (CORK_UNLIKELY(op == NULL)) {
        swan_undefined("Value doesn't contain operation named %s", name);
    }
    return op;
}

CORK_ATTR_UNUSED
static inline int
swan_value_evaluate(struct swan_value *value, const char *name,
                    size_t param_count, struct swan_value *params)
{
    struct swan_operation  *op = swan_value_require_operation(value, name);
    if (CORK_UNLIKELY(op == NULL)) {
        return -1;
    } else {
        return swan_operation_evaluate(op, param_count, params);
    }
}

#define swan_value_unref(value) \
    swan_value_evaluate((value), "~unref", 1, (value))


#endif /* SWANSON_METAMODEL_H */
