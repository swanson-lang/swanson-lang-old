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

#include "swanson/s0.h"

#include "s0.h"


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

static int
swan_s0_verify_operation_call(const char *target, const char *operation_name,
                              size_t param_count, const char **params)
{
    bool  first;
    size_t  i;

    printf("{ \"type\": \"operation_call\", \"target\": ");
    print_quoted_string(target);
    printf(", \"operation\": ");
    print_quoted_string(operation_name);

    printf(", \"params\": [");
    for (first = true, i = 0; i < param_count; i++) {
        if (first) {
            first = false;
        } else {
            printf(", ");
        }
        print_quoted_string(params[i]);
    }

    printf("] }\n");
    return 0;
}

static int
swan_s0_verify_string_constant(const char *result, const char *contents,
                               size_t content_length)
{
    printf("{ \"type\": \"string_constant\", \"result\": ");
    print_quoted_string(result);
    printf(", \"contents\": ");
    print_quoted_string_len(contents, content_length);
    printf(" }\n");
    return 0;
}

static struct swan_s0_callback  swan_s0_verify_callback = {
    swan_s0_verify_operation_call,
    swan_s0_verify_string_constant
};

#define READ_BUF_SIZE  65536
static char  READ_BUF[READ_BUF_SIZE];

static void
swan_s0_verify(int argc, char **argv)
{
    FILE  *file;
    int  rc;
    bool  should_close;
    size_t  bytes_read;
    struct cork_buffer  buf = CORK_BUFFER_INIT();

    /* Open the input file. */
    if (argc == 0) {
        should_close = false;
        file = stdin;
    } else if (argc == 1) {
        should_close = true;
        file = fopen(argv[0], "r");
        if (file == NULL) {
            fprintf(stderr, "Error opening %s:\n  %s\n",
                    argv[0], strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else {
        cork_command_show_help
            (&verify, "Cannot verify more than one input file.");
        exit(EXIT_FAILURE);
    }

    /* Read in the contents of the file. */
    while ((bytes_read = fread(READ_BUF, 1, READ_BUF_SIZE, file)) != 0) {
        cork_buffer_append(&buf, READ_BUF, bytes_read);
    }

    if (ferror(file)) {
        fprintf(stderr, "Error reading from %s:\n  %s\n",
                argv[0], strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Close the file if necessary. */
    if (should_close) {
        fclose(file);
    }

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
 * Subcommand list
 */

static struct cork_command  *subcommands[] = {
    &verify
};

struct cork_command  s0_command =
    cork_command_set("s0", NULL, NULL, subcommands);
