/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef LAGAVULIN_H
#define LAGAVULIN_H

#include <libcork/core.h>


/* Forward declarations */

struct lgv_parameter;
struct lgv_method;
struct lgv_method_set;
struct lgv_ref;


/*-----------------------------------------------------------------------
 * Errors
 */

/* hash of "lagavulin.h" */
#define LGV_ERROR  0x884458ce

enum lgv_error {
    LGV_UNDEFINED
};

#define lgv_set_error(code, ...) (cork_error_set(LGV_ERROR, code, __VA_ARGS__))
#define lgv_undefined(...) \
    lgv_set_error(LGV_UNDEFINED, __VA_ARGS__)


/*-----------------------------------------------------------------------
 * Methods
 */

struct lgv_parameter {
    const char  *name;
    struct lgv_ref  *ref;
};

typedef int
(*lgv_method_invoke_f)(void *user_data, size_t param_count,
                       struct lgv_parameter *params);

struct lgv_method *
lgv_method_new(void *user_data, cork_free_f free_user_data,
               lgv_method_invoke_f invoke);

void
lgv_method_free(struct lgv_method *method);

int
lgv_method_invoke(struct lgv_method *method, size_t param_count,
                  struct lgv_parameter *params);


/*-----------------------------------------------------------------------
 * Method sets
 */

struct lgv_method_set *
lgv_method_set_new(void);

void
lgv_method_set_free(struct lgv_method_set *set);

/* Takes control of method, overwrites any existing method with the same name */
void
lgv_method_set_add(struct lgv_method_set *set, const char *name,
                   struct lgv_method *method);

void
lgv_method_set_add_new(struct lgv_method_set *set, const char *name,
                       void *user_data, cork_free_f free_user_data,
                       lgv_method_invoke_f invoke);

struct lgv_method *
lgv_method_set_get(struct lgv_method_set *set, const char *name);

struct lgv_method *
lgv_method_set_require(struct lgv_method_set *set, const char *name,
                       const char *where);


/*-----------------------------------------------------------------------
 * References
 */

struct lgv_ref {
    void  *value;
    struct lgv_method_set  *methods;
};

#define lgv_ref_clear(ref) ((ref)->value = NULL, (ref)->methods = NULL)


#endif /* LAGAVULIN_H */
