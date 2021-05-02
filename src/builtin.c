#include "builtin.h"

int moveDir(struct command cmd) {
  if (cmd.argc > 1) {
    fprintf(stderr, "pingshell: %s: too many arguments\n", cmd.argv[0]);
    return -1;
  }
  else if(cmd.argc == 1) {
    if (chdir(cmd.argv[1]) == -1) {
      fprintf(stderr, "pingshell: %s: %s: No such file or directory\n", cmd.argv[0], cmd.argv[1]); 
      return -1;
    }
  } else {
    if (chdir(getenv("HOME"))== -1) {
      fprintf(stderr, "pingshell: %s: %s: No such file or directory\n", cmd.argv[0], getenv("HOME")); 
      return -1;
    }
  }
  return 0;
}

void exitShell() {
  kill(getpid(), SIGHUP);
}
