/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef SWANSON_S0_H
#define SWANSON_S0_H

#include <libcork/core.h>


/*-----------------------------------------------------------------------
 * Parsing S0
 */

struct swan_s0_callback {
    int
    (*operation_call)(const char *target, const char *operation_name,
                      size_t param_count, const char **params);

    int
    (*string_constant)(const char *result, const char *contents,
                       size_t content_length);
};


int
swan_s0_parse(const char *buf, size_t len, struct swan_s0_callback *callback);


#endif /* SWANSON_S0_H */
