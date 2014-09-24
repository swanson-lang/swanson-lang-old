/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <assert.h>
#include <stdlib.h>

#include "libcork/core/allocator.h"
#include "s0.h"


struct s0_thing {
    enum s0_kind  kind;
    union {
        struct s0_literal  *literal;
        struct s0_block  *block;
        struct s0_environment  *environment;
        struct s0_type  *type;
    } _;
};


static struct s0_thing *
s0_thing_new(enum s0_kind kind)
{
    struct s0_thing  *thing = cork_new(struct s0_thing);
    thing->kind = kind;
    return thing;
}

void
s0_thing_free(struct s0_thing *thing)
{
    switch (thing->kind) {
        case S0_KIND_ATOM:
            break;
        case S0_KIND_BLOCK:
            s0_block_free(thing->_.block);
            break;
        case S0_KIND_ENVIRONMENT:
            s0_environment_free(thing->_.environment);
            break;
        case S0_KIND_LITERAL:
            s0_literal_free(thing->_.literal);
            break;
        case S0_KIND_TYPE:
            s0_type_free(thing->_.type);
            break;
        default:
            break;
    }
    free(thing);
}

enum s0_kind
s0_thing_kind(struct s0_thing *thing)
{
    return thing->kind;
}


struct s0_thing *
s0_thing_new_atom(void)
{
    return s0_thing_new(S0_KIND_ATOM);
}


struct s0_thing *
s0_thing_new_block(struct s0_block *block)
{
    struct s0_thing  *thing = s0_thing_new(S0_KIND_BLOCK);
    thing->_.block = block;
    return thing;
}

struct s0_block *
s0_thing_block_get(struct s0_thing *thing)
{
    assert(thing->kind == S0_KIND_BLOCK);
    return thing->_.block;
}


struct s0_thing *
s0_thing_new_environment(struct s0_environment *environment)
{
    struct s0_thing  *thing = s0_thing_new(S0_KIND_ENVIRONMENT);
    thing->_.environment = environment;
    return thing;
}

struct s0_environment *
s0_thing_environment_get(struct s0_thing *thing)
{
    assert(thing->kind == S0_KIND_ENVIRONMENT);
    return thing->_.environment;
}


struct s0_thing *
s0_thing_new_literal(struct s0_literal *literal)
{
    struct s0_thing  *thing = s0_thing_new(S0_KIND_LITERAL);
    thing->_.literal = literal;
    return thing;
}

struct s0_literal *
s0_thing_literal_get(struct s0_thing *thing)
{
    assert(thing->kind == S0_KIND_LITERAL);
    return thing->_.literal;
}


struct s0_thing *
s0_thing_new_type(struct s0_type *type)
{
    struct s0_thing  *thing = s0_thing_new(S0_KIND_TYPE);
    thing->_.type = type;
    return thing;
}

struct s0_type *
s0_thing_type_get(struct s0_thing *thing)
{
    assert(thing->kind == S0_KIND_TYPE);
    return thing->_.type;
}
