/*\
 * unixglue.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include "Sudoku.hxx"
#include "unixglue.hxx"

static const char *module = "unixglue";

void Post_Command( WPARAM cmd )
{
    sprtf("TODO: Post_Command( %d ) to be done...\n", (int)cmd );
}


// eof = unixglue.cxx
