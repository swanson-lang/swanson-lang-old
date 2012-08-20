/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libcork/core.h>
#include <libcork/cli.h>
#include <libcork/ds.h>
#include <libcork/helpers/errors.h>

#include "swanson/s0.h"

#include "s0.h"


/*-----------------------------------------------------------------------
 * Helper functions
 */

#define READ_BUF_SIZE  65536
static char  READ_BUF[READ_BUF_SIZE];

static void
read_file(const char *filename, struct cork_buffer *dest)
{
    FILE  *file;
    bool  should_close;
    size_t  bytes_read;

    /* Open the input file. */
    if (filename == NULL) {
        should_close = false;
        file = stdin;
        filename = "<stdin>";
    } else {
        should_close = true;
        file = fopen(filename, "r");
        if (file == NULL) {
            fprintf(stderr, "Error opening %s:\n  %s\n",
                    filename, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* Read in the contents of the file. */
    while ((bytes_read = fread(READ_BUF, 1, READ_BUF_SIZE, file)) != 0) {
        cork_buffer_append(dest, READ_BUF, bytes_read);
    }

    if (ferror(file)) {
        fprintf(stderr, "Error reading from %s:\n  %s\n",
                filename, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Close the file if necessary. */
    if (should_close) {
        fclose(file);
    }
}


/*-----------------------------------------------------------------------
 * swan s0 verify
 */

#define VERIFY_SHORT_DESC \
    "Verify the contents of an S0 file"

#define VERIFY_USAGE_SUFFIX \
    "[<input file>]"

#define VERIFY_HELP_TEXT \
"Reads in an S0 file and verifies that its syntax is correct.  We will\n" \
"output a normalized form of the file's contents.  If there are any\n" \
"syntax errors, we will print out a description of the first one.\n" \
"\n" \
"Options:\n" \
"  [<input file>]\n" \
"    The S0 file to read.  If this option is not present, we'll read\n" \
"    from standard input.\n" \

static void
swan_s0_verify(int argc, char **argv);

static struct cork_command  verify =
    cork_leaf_command("verify",
                      VERIFY_SHORT_DESC,
                      VERIFY_USAGE_SUFFIX,
                      VERIFY_HELP_TEXT,
                      NULL, swan_s0_verify);


static struct cork_buffer  string_buf = CORK_BUFFER_INIT();

static void
print_quoted_string_len(const char *str, size_t len)
{
    const char  *curr;
    cork_buffer_clear(&string_buf);
    for (curr = str; len > 0; curr++, len--) {
        char  ch = *curr;
        if (ch == '"' || ch == '\\') {
            cork_buffer_append(&string_buf, "\\", 1);
            cork_buffer_append(&string_buf, curr, 1);
        } else if (ch == '\f') {
            cork_buffer_append(&string_buf, "\\f", 2);
        } else if (ch == '\n') {
            cork_buffer_append(&string_buf, "\\n", 2);
        } else if (ch == '\r') {
            cork_buffer_append(&string_buf, "\\r", 2);
        } else if (ch == '\t') {
            cork_buffer_append(&string_buf, "\\t", 2);
        } else if (ch == '\n') {
            cork_buffer_append(&string_buf, "\\n", 2);
        } else if (ch >= ' ' && ch <= '~') {
            cork_buffer_append(&string_buf, curr, 1);
        } else {
            cork_buffer_append_printf(&string_buf, "\\x%02x", (int) ch);
        }
    }

    printf("\"%.*s\"", (int) string_buf.size, (char *) string_buf.buf);
}

static void
print_quoted_string(const char *str)
{
    print_quoted_string_len(str, strlen(str));
}

static void
print_string_list(size_t count, const char **strings)
{
    bool  first;
    size_t  i;

    for (first = true, i = 0; i < count; i++) {
        if (first) {
            first = false;
        } else {
            printf(", ");
        }
        print_quoted_string(strings[i]);
    }
}

static int
swan_s0_verify__operation_call(struct swan_s0_callback *callback,
                               const char *target, const char *operation_name,
                               size_t param_count, const char **params)
{

    printf("{ \"type\": \"operation_call\", \"target\": ");
    print_quoted_string(target);
    printf(", \"operation\": ");
    print_quoted_string(operation_name);
    printf(", \"params\": [");
    print_string_list(param_count, params);
    printf("] }\n");
    return 0;
}

static int
swan_s0_verify__string_constant(struct swan_s0_callback *callback,
                                const char *result, const char *contents,
                                size_t content_length)
{
    printf("{ \"type\": \"string_constant\", \"result\": ");
    print_quoted_string(result);
    printf(", \"contents\": ");
    print_quoted_string_len(contents, content_length);
    printf(" }\n");
    return 0;
}

static int
swan_s0_verify__start_block(struct swan_s0_callback *callback,
                            const char *target, size_t param_count,
                            const char **params)
{
    printf("{ \"type\": \"start_block\", \"target\": ");
    print_quoted_string(target);
    printf(", \"params\": [");
    print_string_list(param_count, params);
    printf("] }\n");
    return 0;
}

static int
swan_s0_verify__end_block(struct swan_s0_callback *callback)
{
    printf("{ \"type\": \"end_block\" }\n");
    return 0;
}

static int
swan_s0_verify__finish(struct swan_s0_callback *callback)
{
    return 0;
}

static struct swan_s0_callback  swan_s0_verify_callback = {
    swan_s0_verify__operation_call,
    swan_s0_verify__string_constant,
    swan_s0_verify__start_block,
    swan_s0_verify__end_block,
    swan_s0_verify__finish
};

static void
swan_s0_verify(int argc, char **argv)
{
    int  rc;
    struct cork_buffer  buf = CORK_BUFFER_INIT();

    /* Open the input file. */
    if (argc > 1) {
        cork_command_show_help
            (&verify, "Cannot verify more than one input file.");
        exit(EXIT_FAILURE);
    }

    /* Read in the contents of the file. */
    read_file(argc == 0? NULL: argv[0], &buf);

    /* Parse the file, printing out any error that occurs. */
    swan_s0_parse((char *) buf.buf, buf.size, &swan_s0_verify_callback);
    if (cork_error_occurred()) {
        rc = EXIT_FAILURE;
        fprintf(stderr, "%s\n", cork_error_message());
    } else {
        rc = EXIT_SUCCESS;
    }

    cork_buffer_done(&buf);
    exit(rc);
}


/*-----------------------------------------------------------------------
 * swan s0 evaluate
 */

#define EVALUATE_SHORT_DESC \
    "Evaluate the contents of an S0 file"

#define EVALUATE_USAGE_SUFFIX \
    "[<input file>]"

#define EVALUATE_HELP_TEXT \
"Reads in an S0 file and evaluates it.\n" \
"\n" \
"Options:\n" \
"  [<input file>]\n" \
"    The S0 file to read.  If this option is not present, we'll read\n" \
"    from standard input.\n" \

static void
swan_s0_evaluate(int argc, char **argv);

static struct cork_command  evaluate =
    cork_leaf_command("evaluate",
                      EVALUATE_SHORT_DESC,
                      EVALUATE_USAGE_SUFFIX,
                      EVALUATE_HELP_TEXT,
                      NULL, swan_s0_evaluate);

static void
swan_s0_evaluate(int argc, char **argv)
{
    int  rc;
    struct cork_buffer  buf = CORK_BUFFER_INIT();
    struct swan_s0_evaluator  *eval;

    /* Open the input file. */
    if (argc > 1) {
        cork_command_show_help
            (&verify, "Cannot verify more than one input file.");
        exit(EXIT_FAILURE);
    }

    /* Read in the contents of the file. */
    read_file(argc == 0? NULL: argv[0], &buf);

    /* Evaluate the file, printing out any error that occurs. */
    eval = swan_s0_evaluator_new_kernel();
    swan_s0_parse((char *) buf.buf, buf.size, &eval->callback);
    if (cork_error_occurred()) {
        rc = EXIT_FAILURE;
        fprintf(stderr, "%s\n", cork_error_message());
    } else {
        rc = EXIT_SUCCESS;
    }

    swan_s0_evaluator_free(eval);
    cork_buffer_done(&buf);
    cork_gc_done();
    exit(rc);
}


/*-----------------------------------------------------------------------
 * Subcommand list
 */

static struct cork_command  *subcommands[] = {
    &evaluate,
    &verify,
    NULL
};

struct cork_command  s0_command =
    cork_command_set("s0", NULL, NULL, subcommands);
