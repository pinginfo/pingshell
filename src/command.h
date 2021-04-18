#ifndef COMMANDS_H_
#define COMMANDS_H_
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <linux/limits.h>
#include <string.h>

#define SIZE 255 

typedef char** my_argv;

typedef struct command {
  int argc;
  char** argv;
  char* output;
  int background_task;
  struct command* next_command;
} command;

void free_command(struct command *cmd);
int bufferToCommand(char *input, struct command *cmd);
#endif
