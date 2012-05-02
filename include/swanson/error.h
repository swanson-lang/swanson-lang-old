/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef SWANSON_ERROR_H
#define SWANSON_ERROR_H

#include <libcork/core.h>

/* Hash of "swanson" */
#define SWAN_ERROR  0xef08a1b8

enum swan_error {
    SWAN_UNDEFINED,
    SWAN_S0_PARSE_ERROR,
    SWAN_S0_UNDEFINED_IDENTIFIER
};

#define swan_set_error(code, ...) \
    (cork_error_set(SWAN_ERROR, code, __VA_ARGS__))
#define swan_undefined(...) \
    swan_set_error(SWAN_UNDEFINED, __VA_ARGS__)
#define swan_s0_parse_error(...) \
    swan_set_error(SWAN_S0_PARSE_ERROR, __VA_ARGS__)
#define swan_s0_undefined_identifier(...) \
    swan_set_error(SWAN_S0_UNDEFINED_IDENTIFIER, __VA_ARGS__)

#endif /* SWANSON_ERROR_H */
