#include "builtin.h"
#include "command.h"
#include "jobs.h"
#include "signals.h"
#include <signal.h>
#include <stdio.h>
#include <stdint.h>

#define debug 0

int main() {
  char buff[STRING_MAX_SIZE];
  struct command cmd;
  struct sigaction sa;

  if (init_handler(sa, SA_RESTART | SA_SIGINFO) == -1) { exit(EXIT_FAILURE); }

  while(1) {
    printDir();
    if (fgets(buff, STRING_MAX_SIZE, stdin) != NULL) {
      if (bufferToCommand(buff, &cmd) == -1) {
        fprintf(stderr, "cannot read the command\n");
        free_command(cmd);
        continue;
      }
#if debug==1
      print_command(cmd);
#endif
      if (strncmp(cmd.argv[0], "\n", 1) == 0) {
        free_command(cmd);
        continue;
      } else if (strncmp(cmd.argv[0], "exit", 5) == 0) {
        free_command(cmd);
        exitShell();
      } else if (strncmp(cmd.argv[0], "cd", 2) == 0) { moveDir(cmd); }
      else {
        if (execCommand(cmd) == -1) {
          fprintf(stderr, "error execCommand");
        }
      }
      free_command(cmd);
    }
  }
}
