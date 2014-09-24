/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef S0_H
#define S0_H

#include <stdlib.h>


/* C types */

struct s0_block;
struct s0_environment;
struct s0_literal;
struct s0_statement;
struct s0_thing;
struct s0_type;


/*-----------------------------------------------------------------------
 * Things
 */

enum s0_kind {
    S0_KIND_ATOM,
    S0_KIND_BLOCK,
    S0_KIND_ENVIRONMENT,
    S0_KIND_LITERAL,
    S0_KIND_TYPE
};

void
s0_thing_free(struct s0_thing *thing);

enum s0_kind
s0_thing_kind(struct s0_thing *thing);


struct s0_thing *
s0_thing_new_atom(void);


/* Takes control of block */
struct s0_thing *
s0_thing_new_block(struct s0_block *block);

/* Undefined if thing isn't a block */
struct s0_block *
s0_thing_block_get(struct s0_thing *thing);


/* Takes control of environment */
struct s0_thing *
s0_thing_new_environment(struct s0_environment *environment);

/* Undefined if thing isn't an environment */
struct s0_environment *
s0_thing_environment_get(struct s0_thing *thing);


/* Takes control of literal */
struct s0_thing *
s0_thing_new_literal(struct s0_literal *literal);

/* Undefined if thing isn't a literal */
struct s0_literal *
s0_thing_literal_get(struct s0_thing *thing);


/* Takes control of type */
struct s0_thing *
s0_thing_new_type(struct s0_type *type);

/* Undefined if thing isn't a type */
struct s0_type *
s0_thing_type_get(struct s0_thing *thing);


/*-----------------------------------------------------------------------
 * Block specs
 */

struct s0_block_spec_entry {
    struct s0_type  *input_type;
    struct s0_type  *output_type;
};

struct s0_block_spec *
s0_block_spec_new(void);

void
s0_block_spec_free(struct s0_block_spec *spec);

/* Returns NULL if entry doesn't exist. */
struct s0_block_spec_entry *
s0_block_spec_get(struct s0_block_spec *spec,
                  size_t key_size, const void *key);

/* Returns whether entry is new; frees any existing entry.  Takes control of
 * input_type and output_type. */
int
s0_block_spec_put(struct s0_block_spec *spec,
                  size_t key_size, const void *key,
                  struct s0_type *input_type, struct s0_type *output_type);


/*-----------------------------------------------------------------------
 * Blocks
 */

/* Takes control of spec */
struct s0_block *
s0_block_new(struct s0_block_spec *spec);

void
s0_block_free(struct s0_block *block);

size_t
s0_block_statement_count(struct s0_block *block);

struct s0_statement *
s0_block_get_statement(struct s0_block *block, size_t index);

void
s0_block_add_statement(struct s0_block *block, struct s0_statement *stmt);


/*-----------------------------------------------------------------------
 * Environments
 */

struct s0_environment *
s0_environment_new(void);

void
s0_environment_free(struct s0_environment *environment);

/* Returns NULL if entry doesn't exist. */
struct s0_thing *
s0_environment_get(struct s0_environment *environment,
                   size_t key_size, const void *key);

/* Returns whether entry is new; frees any existing entry.  Takes control of
 * value. */
int
s0_environment_put(struct s0_environment *environment,
                   size_t key_size, const void *key, struct s0_thing *value);


/*-----------------------------------------------------------------------
 * Literals
 */

typedef void
(s0_literal_free_f)(const void *content);

/* Takes control of content; we'll eventually free it using the provided
 * function. */
struct s0_literal *
s0_literal_new(size_t size, const void *content, s0_literal_free_f *free);

void
s0_literal_free(struct s0_literal *literal);

size_t
s0_literal_get_size(struct s0_literal *literal);

const void *
s0_literal_get_content(struct s0_literal *literal);


/*-----------------------------------------------------------------------
 * Types
 */

struct s0_type *
s0_type_new(void);

void
s0_type_free(struct s0_type *type);


#endif /* S0_H */
