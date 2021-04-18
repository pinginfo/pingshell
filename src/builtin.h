#ifndef BUILTIN_H
#define BUILTIN_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "command.h"

void moveDir(struct command cmd);
void exitShell();
#endif
