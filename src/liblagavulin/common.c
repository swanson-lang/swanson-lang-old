/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <libcork/core.h>
#include <libcork/helpers/errors.h>

#include "lagavulin.h"


int
lgv_unref(struct lgv_ref *ref, const char *where)
{
    struct lgv_method  *method;
    struct lgv_parameter  params[1];
    rip_check(method = lgv_method_set_require(ref->methods, "~unref", where));
    params[0].name = "ref";
    params[0].ref = ref;
    return lgv_method_invoke(method, 1, params);
}
