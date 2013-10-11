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


/* Forward declarations */

struct s0_formals;
struct s0_ref_type;
struct s0_type;
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


#endif /* S0_H */
