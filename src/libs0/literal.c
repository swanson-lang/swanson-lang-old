/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>

#include "libcork/core/allocator.h"
#include "s0.h"


struct s0_literal {
    size_t  size;
    const void  *content;
    s0_literal_free_f  *free;
};


struct s0_literal *
s0_literal_new(size_t size, const void *content, s0_literal_free_f *free)
{
    struct s0_literal  *literal = cork_new(struct s0_literal);
    literal->size = size;
    literal->content = content;
    literal->free = free;
    return literal;
}

void
s0_literal_free(struct s0_literal *literal)
{
    if (literal->free != NULL) {
        literal->free(literal->content);
    }
    free(literal);
}

size_t
s0_literal_get_size(struct s0_literal *literal)
{
    return literal->size;
}

const void *
s0_literal_get_content(struct s0_literal *literal)
{
    return literal->content;
}
