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
#include <libcork/os.h>
#include <libcork/threads.h>


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
    LGV_BAD_METHOD_CALL,
    LGV_UNDEFINED
};

#define lgv_set_error(code, ...) (cork_error_set(LGV_ERROR, code, __VA_ARGS__))
#define lgv_bad_method_call(...) \
    lgv_set_error(LGV_BAD_METHOD_CALL, __VA_ARGS__)
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

struct lgv_ref *
lgv_parameter_get_ref(const char *name, size_t param_count,
                      struct lgv_parameter *params);

void *
lgv_parameter_get_value(const char *name, size_t param_count,
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

#define lgv_define_simple_method_set(name) \
static void \
name##__method_set_fill(struct lgv_method_set *set); \
\
static struct lgv_method_set  *name##__method_set; \
cork_once_barrier(name); \
\
static void \
name##__method_set_done(void) \
{ \
    lgv_method_set_free(name##__method_set); \
} \
\
static void \
name##__method_set_init(void) \
{ \
    name##__method_set = lgv_method_set_new(); \
    name##__method_set_fill(name##__method_set); \
    cork_cleanup_at_exit(0, name##__method_set_done); \
} \
\
static struct lgv_method_set * \
name##__method_set_new(void) \
{ \
    cork_once(name, name##__method_set_init()); \
    return name##__method_set; \
} \
\
static void \
name##__method_set_fill(struct lgv_method_set *set)

#define lgv_simple_method_set_new(name)        (name##__method_set_new())
#define lgv_simple_method_set_free(name, set)  /* do nothing */


/*-----------------------------------------------------------------------
 * References
 */

struct lgv_ref {
    void  *value;
    struct lgv_method_set  *methods;
};

#define lgv_ref_clear(ref) ((ref)->value = NULL, (ref)->methods = NULL)


/*-----------------------------------------------------------------------
 * Common method invocations
 */

/* ~unref(ref → ø) */
int
lgv_unref(struct lgv_ref *ref, const char *where);


/*-----------------------------------------------------------------------
 * Built-in types
 */

int
lgv_ref_new_size(struct lgv_ref *ref, size_t initial_value);


#endif /* LAGAVULIN_H */
