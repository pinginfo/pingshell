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
int execCommand(struct command cmd);
#endif
