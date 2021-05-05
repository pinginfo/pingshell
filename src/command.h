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

/**
 * Print the command for debugging
 *
 * @param A command
 */
void print_command(struct command cmd);

/**
 * Free all pointers of the command
 *
 * @param A command
 * @note Free the pointers of the commands of the chain also
 */
void free_command(struct command cmd);

/**
 * Convert a string into a command
 *        the command can be :
 *          - simple command,
 *          - simple command with a redirection in a file
 *          - simple command in background
 *          - simple command with a redirection in  a file and in background
 *          - a pipe command (max 2 commands)
 *          - a pipe command (max 2 commands) with a redirection in a file
 *          - a pipe command (max 2 commands) in background
 *
 * @note Be careful to put spaces :
 *        sleep 1 | sleep 1 --> Ok
 *        sleep 1 |sleep 1 ---> Not ok
 * @param The intial string, the command to return
 * @return 0 is success, -1 in error && return the command in the second argument
 */
int bufferToCommand(char *input, struct command *cmd);
#endif
