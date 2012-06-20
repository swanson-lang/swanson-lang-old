/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef SWANSON_HELPERS_METAMODEL_H
#define SWANSON_HELPERS_METAMODEL_H

#include <assert.h>
#include <string.h>

#include <libcork/core.h>
#include <libcork/helpers/errors.h>

#include <swanson/metamodel.h>


#define _static_op_(opset, name) \
static int \
opset##__##name##__eval(struct swan_operation *operation, size_t param_count, \
                        struct swan_value *params); \
static struct swan_operation  opset##__##name##__op = { \
    opset##__##name##__eval \
}; \
static int \
opset##__##name##__eval(struct swan_operation *operation, size_t param_count, \
                        struct swan_value *params)


#define _constant_op_(_opset, _name, ret_opset, ret_content) \
_static_op_(_opset, _name) \
{ \
    assert(param_count == 1); \
    assert(swan_value_is_empty(&params[0])); \
    params[0].opset = ret_opset; \
    params[0].content = ret_content; \
    return 0; \
}


CORK_ATTR_UNUSED
static void
swan_opset__static__unref(struct swan_opset *opset)
{
    /* For static objects, there's nothing to unreference. */
}

#define _get_operation_(name) \
static struct swan_operation * \
name##__get_operation(struct swan_opset *opset, const char *op_name)

#define _opset_(name, linkage) \
_get_operation_(name); \
linkage struct swan_opset  name##__opset = { \
    name##__get_operation, \
    swan_opset__static__unref \
}; \
_get_operation_(name)

#define _extern_opset_(name)  _opset_(name, )
#define _static_opset_(name)  _opset_(name, static)


#define try_get_operation(other) \
    do { \
        struct swan_operation  *__op = \
            other##__get_operation(NULL, op_name); \
        if (__op != NULL) { \
            return __op; \
        } \
    } while (0)

#define get_static_op(name, opset, c_name) \
    do { \
        if (strcmp(op_name, name) == 0) { \
            return &opset##__##c_name##__op; \
        } \
    } while (0)


#endif /* SWANSON_HELPERS_METAMODEL_H */
