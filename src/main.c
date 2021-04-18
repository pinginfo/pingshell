#include "builtin.h"
#include "command.h"
#include "jobs.h"
#include <signal.h>

void mon_handler(int signum, siginfo_t* info, void* mydata) {
  switch (signum) {
    case SIGCHLD:
      write(STDOUT_FILENO, "Background job exited\n", 22);
      break;
    default:
      break;
  }
}


void printDir() {
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  printf("%s ", cwd);
}

int main(int argc, char *argv[]) {
  char buff[SIZE];
  struct command cmd;
  struct sigaction sa;
  sa.sa_sigaction = mon_handler;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGUSR1);
  sigaddset(&sa.sa_mask, SIGTERM);
  sa.sa_flags = 0;
  /*
     if (sigaction(SIGCHLD, &sa, &pid_bg_task) == -1) {
     perror("setting up SIGCHLD");
     return 1;
     }
     */

  while(1) {
    printf("pingshell# ");
    printDir();
    if (fgets(buff, SIZE, stdin) != NULL) {
      if (bufferToCommand(buff, &cmd) == -1) {
        fprintf(stderr, "cannot read the command\n");
        free_command(&cmd);
        continue;
      }
      if (strncmp(cmd.argv[0], "exit", 5) == 0) exitShell();
      else if (strncmp(cmd.argv[0], "cd", 2) == 0) moveDir(cmd);
      else execCommand(cmd);
      free_command(&cmd);
    }
  }

  free_command(&cmd);
  return 0;
}
