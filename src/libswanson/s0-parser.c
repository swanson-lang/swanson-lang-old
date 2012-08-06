/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <libcork/core.h>
#include <libcork/ds.h>
#include <libcork/helpers/errors.h>

#include "swanson/error.h"
#include "swanson/metamodel.h"
#include "swanson/s0.h"


#if !defined(SWAN_DEBUG_PARSER)
#define SWAN_DEBUG_PARSER  0
#endif

#if SWAN_DEBUG_PARSER
#include <stdio.h>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...) /* no debug messages */
#endif


/* A "miss" happens when we try to parse a particular token or nonterminal, and
 * something else is present.  This may or may not become a full parse error.  A
 * "failure" happens when we're in the middle of a token and something
 * definitely incorrect occurs. */
#define PARSE_SUCCESS  0
#define PARSE_MISS  1
#define PARSE_FAILURE  -1


typedef cork_array(struct cork_buffer)  buffer_array;
typedef cork_array(const char *)  string_array;

struct buf {
    const char  *data;
    size_t  len;
    struct swan_s0_callback  *callback;

    const char  *target;
    const char  *operation_name;

    size_t  next_buffer;
    buffer_array  buffers;
    string_array  params;
};

#define BUF_LEN  (buf->len)
#define BUF_HEAD  (buf->data[0])
#define ADVANCE_BUF  (buf->data++, buf->len--)

#define IS_ALPHA(ch) \
    (((ch) >= 'a' && (ch) <= 'z') || \
     ((ch) >= 'A' && (ch) <= 'Z') || \
     ((ch) == '_') || \
     ((ch) == '~'))

#define IS_ALNUM(ch) \
    (IS_ALPHA(ch) || ((ch) >= '0' && (ch) <= '9'))

#define IS_SPACE(ch) \
    ((ch) == ' '  || (ch) == '\t' || (ch) == '\n' || \
     (ch) == '\v' || (ch) == '\f' || (ch) == '\r')

#define PARSE_ERROR(...) \
    do { \
        swan_s0_parse_error(__VA_ARGS__); \
        DEBUG("Parse error: %s\n", cork_error_message()); \
        return PARSE_FAILURE; \
    } while (0)

#define TRY(call) \
    do { \
        int  rc = call; \
        if (rc != PARSE_MISS) { \
            return rc; \
        } \
    } while (0)

#define OPERATION_CALLBACK \
    (buf->callback->operation_call \
        (buf->callback, buf->target, buf->operation_name, \
         cork_array_size(&buf->params), &cork_array_at(&buf->params, 0)))

#define STRING_CALLBACK \
    (buf->callback->string_constant \
        (buf->callback, cork_array_at(&buf->params, 0), \
         contents, content_length))

static inline struct cork_buffer *
new_buffer(struct buf *buf)
{
    struct cork_buffer  *result;

    /* If we already used all the buffers we've allocated so far, allocate
     * another one. */
    if (buf->next_buffer >= cork_array_size(&buf->buffers)) {
        result = cork_array_append_get(&buf->buffers);
        cork_buffer_init(result);
    } else {
        result = &cork_array_at(&buf->buffers, buf->next_buffer);
    }

    buf->next_buffer++;
    return result;
}

static inline void
skip_whitespace(struct buf *buf)
{
    while (BUF_LEN >= 1 && IS_SPACE(BUF_HEAD)) {
        ADVANCE_BUF;
    }
}

static inline int
swan_parse_quoted_string(struct buf *buf, struct cork_buffer *dest)
{
    DEBUG("  Trying to parse quoted string\n");
    skip_whitespace(buf);
    cork_buffer_clear(dest);

    if (BUF_LEN < 1 || BUF_HEAD != '"') return PARSE_MISS;

    ADVANCE_BUF;
    while (BUF_LEN >= 1) {
        if (BUF_HEAD == '"') {
            ADVANCE_BUF;
            DEBUG("    Got %s\n", (char *) dest->buf);
            return PARSE_SUCCESS;
        }

        if (BUF_HEAD == '\\') {
            ADVANCE_BUF;
            if (BUF_LEN < 1) {
                PARSE_ERROR("Unterminated string constant");
            }
        }

        cork_buffer_append(dest, &BUF_HEAD, 1);
        ADVANCE_BUF;
    }

    PARSE_ERROR("Unterminated string constant");
}

static inline int
swan_parse_id_token(struct buf *buf, struct cork_buffer *dest)
{
    const char  *start;

    DEBUG("  Trying to parse unquoted identifier\n");
    skip_whitespace(buf);

    if (BUF_LEN < 1 || !IS_ALPHA(BUF_HEAD)) {
        return PARSE_MISS;
    }

    start = &BUF_HEAD;
    while (BUF_LEN >= 1 && IS_ALNUM(BUF_HEAD)) {
        ADVANCE_BUF;
    }
    cork_buffer_set(dest, start, &BUF_HEAD - start);
    DEBUG("    Got %s\n", (char *) dest->buf);
    return PARSE_SUCCESS;
}

static inline int
swan_parse_identifier(struct buf *buf, struct cork_buffer *dest)
{
    DEBUG("Trying to parse identifier\n");
    TRY(swan_parse_quoted_string(buf, dest));
    TRY(swan_parse_id_token(buf, dest));
    PARSE_ERROR("Expected identifier");
}

static inline char
swan_peek_token(struct buf *buf)
{
    skip_whitespace(buf);
    return (BUF_LEN >= 1)? BUF_HEAD: '\0';
}

static inline char
swan_get_token(struct buf *buf)
{
    skip_whitespace(buf);
    if (BUF_LEN >= 1) {
        char  ch = BUF_HEAD;
        ADVANCE_BUF;
        return ch;
    } else {
        return '\0';
    }
}

static inline int
swan_require_token(struct buf *buf, char token)
{
    char  ch = swan_get_token(buf);
    if (ch == token) {
        return PARSE_SUCCESS;
    } else {
        PARSE_ERROR("Expected %c", token);
    }
}

static inline int
swan_parse_operation_params(struct buf *buf)
{
    char  token;
    bool  first_param = true;

    while (true) {
        if (first_param) {
            token = swan_peek_token(buf);
            if (token == ')') {
                /* Read a semicolon and then we're done with the call. */
                ADVANCE_BUF;
                rii_check(swan_require_token(buf, ';'));
                return OPERATION_CALLBACK;
            } else if (token == '"' || IS_ALPHA(token)) {
                struct cork_buffer  *id = new_buffer(buf);
                rii_check(swan_parse_identifier(buf, id));
                cork_array_append(&buf->params, id->buf);
                first_param = false;
            } else {
                PARSE_ERROR("Expected \")\" or identifier");
            }
        } else {
            token = swan_get_token(buf);
            if (token == ')') {
                /* Read a semicolon and then we're done with the call. */
                rii_check(swan_require_token(buf, ';'));
                return OPERATION_CALLBACK;
            } else if (token == ',') {
                struct cork_buffer  *id = new_buffer(buf);
                rii_check(swan_parse_identifier(buf, id));
                cork_array_append(&buf->params, id->buf);
            } else {
                PARSE_ERROR("Expected \",\" or \")\"");
            }
        }
    }
}

static inline int
swan_parse_operation_call(struct buf *buf)
{
    char  token;
    struct cork_buffer  *id;
    DEBUG("--- Rest of operation call\n");

    /* We'll have already parsed up through the ".".  Next is the identifier for
     * the operation name. */
    id = new_buffer(buf);
    rii_check(swan_parse_identifier(buf, id));
    buf->operation_name = id->buf;

    /* There may or may not be a parameter list. */
    token = swan_get_token(buf);
    if (token == ';') {
        /* This operation call is done. */
        return OPERATION_CALLBACK;
    } else if (token == '(') {
        return swan_parse_operation_params(buf);
    } else {
        PARSE_ERROR("Expected \"(\" or \";\"");
    }
}

static inline int
swan_parse_result_list(struct buf *buf)
{
    /* We'll have already parsed up through the "," */
    while (true) {
        char  token;
        struct cork_buffer  *id = new_buffer(buf);

        /* Parse the next identifier */
        rii_check(swan_parse_identifier(buf, id));
        cork_array_append(&buf->params, id->buf);

        /* See if there are more results */
        token = swan_get_token(buf);
        if (token == '=') {
            /* After the = must be a operation call. */
            id = new_buffer(buf);
            rii_check(swan_parse_identifier(buf, id));
            buf->target = id->buf;
            token = swan_get_token(buf);
            if (token == '.') {
                return swan_parse_operation_call(buf);
            } else if (token == ':') {
                cork_array_append(&buf->params, id->buf);
                return swan_parse_operation_call(buf);
            } else {
                PARSE_ERROR("Expected \".\" or \":\"");
            }
        } else if (token != ',') {
            PARSE_ERROR("Expected \",\" or \"=\"");
        }
    }
}

static inline int
swan_parse_one_result(struct buf *buf)
{
    char  token;

    /* We'll have parsed up through the "=" */
    token = swan_peek_token(buf);
    if (token == '"') {
        /* This could either be a operation call or a string constant. */
        struct cork_buffer  *id = new_buffer(buf);
        rii_check(swan_parse_quoted_string(buf, id));
        token = swan_get_token(buf);
        if (token == ';') {
            /* It was a string constant */
            const char  *contents = (id->buf == NULL)? "": id->buf;
            size_t  content_length = id->size;
            return STRING_CALLBACK;
        } else if (token == '.') {
            /* It's a operation call */
            buf->target = id->buf;
            return swan_parse_operation_call(buf);
        } else if (token == ':') {
            /* It's a operation call with an implicit "self" parameter */
            buf->target = id->buf;
            cork_array_append(&buf->params, id->buf);
            return swan_parse_operation_call(buf);
        } else {
            PARSE_ERROR("Expected \".\", \":\", or \";\"");
        }
    } else if (IS_ALPHA(token)) {
        struct cork_buffer  *id = new_buffer(buf);
        rii_check(swan_parse_id_token(buf, id));
        buf->target = id->buf;
        token = swan_get_token(buf);
        if (token == '.') {
            return swan_parse_operation_call(buf);
        } else if (token == ':') {
            cork_array_append(&buf->params, id->buf);
            return swan_parse_operation_call(buf);
        } else {
            PARSE_ERROR("Expected \".\" or \":\"");
        }
        return swan_parse_operation_call(buf);
    } else {
        PARSE_ERROR("Expected identifier or string constant");
    }
}

static inline int
swan_parse_statement(struct buf *buf)
{
    char  token;
    struct cork_buffer  *id;

    /* Reset things */
    buf->next_buffer = 0;
    buf->params.size = 0;

    DEBUG("--- Parsing statement\n");

    /* All statements start with at least one identifier. */
    id = new_buffer(buf);
    rii_check(swan_parse_identifier(buf, id));

    /* Then next token determines what happens next. */
    token = swan_get_token(buf);
    if (token == ',') {
        /* We've got a list of results for a operation call. */
        DEBUG("--- List of results\n");
        cork_array_append(&buf->params, id->buf);
        return swan_parse_result_list(buf);
    } else if (token == '=') {
        /* We've got a single result, for either a operation call or a string
         * constant. */
        DEBUG("--- Single result\n");
        cork_array_append(&buf->params, id->buf);
        return swan_parse_one_result(buf);
    } else if (token == '.') {
        /* We've got zero results for a operation call. */
        DEBUG("--- No results\n");
        buf->target = id->buf;
        return swan_parse_operation_call(buf);
    } else if (token == ':') {
        /* We've got zero results for a operation call, and an implicit "self"
         * parameter. */
        DEBUG("--- No results, with self parameter\n");
        buf->target = id->buf;
        cork_array_append(&buf->params, id->buf);
        return swan_parse_operation_call(buf);
    } else {
        PARSE_ERROR("Expected \"=\", \".\", \":\", or \",\"");
    }
}

int
swan_s0_parse(const char *buf, size_t len, struct swan_s0_callback *callback)
{
    int  rc;
    size_t  i;
    struct buf  state;

    state.data = buf;
    state.len = len;
    state.callback = callback;
    state.next_buffer = 0;
    cork_array_init(&state.buffers);
    cork_array_init(&state.params);

    do {
        rc = swan_parse_statement(&state);
        skip_whitespace(&state);
    } while (rc == 0 && state.len > 0);

    for (i = 0; i < cork_array_size(&state.buffers); i++) {
        cork_buffer_done(&cork_array_at(&state.buffers, i));
    }
    cork_array_done(&state.buffers);
    cork_array_done(&state.params);

    if (rc == 0) {
        return callback->finish(callback);
    } else {
        return rc;
    }
}
