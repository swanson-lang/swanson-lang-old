/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef S0_H
#define S0_H

#include <libcork/core.h>
#include <libcork/ds.h>


/* Forward declarations */

struct s0_formals;
struct s0_interface_type;
struct s0_ref_type;
struct s0_value_type;


/*-----------------------------------------------------------------------
 * Errors
 */

/* hash of "s0.h" */
#define S0_ERROR  0x8025cf3d

enum s0_error {
    S0_REDEFINED,
    S0_UNDEFINED
};

#define s0_set_error(code, ...) (cork_error_set(S0_ERROR, code, __VA_ARGS__))
#define s0_redefined(...) \
    s0_set_error(S0_REDEFINED, __VA_ARGS__)
#define s0_undefined(...) \
    s0_set_error(S0_UNDEFINED, __VA_ARGS__)


/*-----------------------------------------------------------------------
 * to_string helpers
 */

CORK_ATTR_UNUSED
static void
s0_indent(struct cork_buffer *dest, size_t indent)
{
    static const char  INDENT[] = "        "; /* 8 spaces */
    size_t  i;
    for (i = 0; i < (indent / 8); i++) {
        cork_buffer_append(dest, INDENT, 8);
    }
    if ((indent % 8) != 0) {
        cork_buffer_append(dest, INDENT, (indent % 8));
    }
}


/*-----------------------------------------------------------------------
 * Type variables
 */

typedef unsigned int  s0_type_var;


/*-----------------------------------------------------------------------
 * Interface types
 */

enum s0_interface_type_kind {
    S0_INTERFACE_TYPE_VAR,
    S0_INTERFACE_TYPE_METHOD_SET
};

struct s0_method {
    const char  *name;
    struct s0_formals  *params;
};

void
s0_interface_type_free(struct s0_interface_type *itype);

enum s0_interface_type_kind
s0_interface_type_kind(const struct s0_interface_type *itype);

void
s0_interface_type_to_string(const struct s0_interface_type *itype,
                            struct cork_buffer *dest, size_t indent);


struct s0_interface_type *
s0_interface_type_new_var(s0_type_var var);

s0_type_var
s0_interface_type_var(const struct s0_interface_type *itype);


struct s0_interface_type *
s0_interface_type_new_method_set(const char *name);

/* Takes control of params, error if method already exists */
int
s0_interface_type_add_method(struct s0_interface_type *itype,
                             const char *method_name,
                             struct s0_formals *params);

const struct s0_method *
s0_interface_type_get_method(struct s0_interface_type *itype,
                             const char *method_name);

const struct s0_method *
s0_interface_type_require_method(struct s0_interface_type *itype,
                                 const char *method_name);


/*-----------------------------------------------------------------------
 * Value types
 */

enum s0_value_type_kind {
    S0_VALUE_TYPE_ANY,
    S0_VALUE_TYPE_VAR
};

void
s0_value_type_free(struct s0_value_type *vtype);

enum s0_value_type_kind
s0_value_type_kind(const struct s0_value_type *vtype);

void
s0_value_type_to_string(const struct s0_value_type *vtype,
                        struct cork_buffer *dest, size_t indent);


struct s0_value_type *
s0_value_type_new_any(void);


struct s0_value_type *
s0_value_type_new_var(s0_type_var var);

s0_type_var
s0_value_type_var(const struct s0_value_type *vtype);


/*-----------------------------------------------------------------------
 * Reference types
 */

/* Takes control of vtype and itype */
struct s0_ref_type *
s0_ref_type_new(struct s0_value_type *vtype, struct s0_interface_type *itype);

void
s0_ref_type_free(struct s0_ref_type *rtype);

const struct s0_value_type *
s0_ref_type_value_type(const struct s0_ref_type *rtype);

const struct s0_interface_type *
s0_ref_type_interface_type(const struct s0_ref_type *rtype);

void
s0_ref_type_to_string(const struct s0_ref_type *rtype,
                      struct cork_buffer *dest, size_t indent);


/*-----------------------------------------------------------------------
 * Formal parameter sets
 */

struct s0_formal {
    const char  *name;
    struct s0_ref_type  *pre;
    struct s0_ref_type  *post;
};

struct s0_formals *
s0_formals_new(const char *name);

void
s0_formals_free(struct s0_formals *formals);

/* Takes control of pre and post, error if parameter already exists */
int
s0_formals_add(struct s0_formals *formals, const char *param_name,
               struct s0_ref_type *pre, struct s0_ref_type *post);

const struct s0_formal *
s0_formals_get(const struct s0_formals *formals, const char *param_name);

const struct s0_formal *
s0_formals_require(const struct s0_formals *formals, const char *param_name);

typedef int
(*s0_formal_map_f)(void *user_data, const struct s0_formal *formal);

int
s0_formals_map(const struct s0_formals *formals, s0_formal_map_f map,
               void *user_data);

void
s0_formals_to_string(const struct s0_formals *formals,
                     struct cork_buffer *dest, size_t indent);


#endif /* S0_H */
