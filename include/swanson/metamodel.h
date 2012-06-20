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

struct swan_value;

struct swan_operation {
    int
    (*evaluate)(struct swan_operation *op, size_t param_count,
                struct swan_value **params);
};

#define swan_operation_evaluate(op, pc, p) \
    ((op)->evaluate((op), (pc), (p)))

struct swan_opset {
    struct swan_operation *
    (*get_operation)(struct swan_opset *opset, const char *name);

    void
    (*unref)(struct swan_opset *opset);
};

#define swan_opset_get_operation(opset, name) \
    ((opset)->get_operation((opset), (name)))

#define swan_opset_unref(opset) \
    ((opset)->unref((opset)))

struct swan_value {
    void  *content;
    struct swan_opset  *opset;
};

CORK_ATTR_UNUSED
static int
swan_value_evaluate(struct swan_value *value, const char *name,
                    size_t param_count, struct swan_value **params)
{
    struct swan_operation  *op = swan_opset_get_operation(value->opset, name);
    if (CORK_UNLIKELY(op == NULL)) {
        return -1;
    } else {
        return swan_operation_evaluate(op, param_count, params);
    }
}


#endif /* SWANSON_METAMODEL_H */
