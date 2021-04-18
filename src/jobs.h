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

static int pid_bg_task;
int execCommand(struct command cmd);
#endif
