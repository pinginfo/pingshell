#ifndef JOBS_H
#define JOBS_H
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "command.h"

extern int pid_chld;
extern int fg_pid;

/**
 * Execute a command
 *
 * @param The command to executed,
 *        the command can be :
 *          - simple command,
 *          - simple command with a redirection in a file
 *          - simple command in background
 *          - simple command with a redirection in  a file and in background
 *          - a pipe command (max 2 commands)
 * @return 0 is success, -1 in error
 */
int execCommand(struct command cmd);
#endif
