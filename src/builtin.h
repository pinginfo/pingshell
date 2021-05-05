#ifndef BUILTIN_H
#define BUILTIN_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "command.h"

/**
 * Allows you to change your current directory
 *
 * @param The cd command with path
 * @note If the path's empty, go to the home
 * @return 0 on success, -1 on error
 */
int moveDir(struct command cmd);

/**
 * Exit the shell and all it's processes
 */
void exitShell();


/**
 * Print the current directory
 */
void printDir();
#endif
