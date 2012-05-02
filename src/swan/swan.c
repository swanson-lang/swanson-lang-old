/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

#include <libcork/cli.h>


/*-----------------------------------------------------------------------
 * Command list
 */

static struct cork_command  *subcommands[] = { NULL };

static struct cork_command  root_command =
    cork_command_set("swan", NULL, NULL, subcommands);


/*-----------------------------------------------------------------------
 * Entry point
 */

int
main(int argc, char **argv)
{
    return cork_command_main(&root_command, argc, argv);
}
