#include "builtin.h"
#include "command.h"
#include "jobs.h"
#include <signal.h>
#include <stdio.h>
#include <stdint.h>

void mon_handler(int signum, siginfo_t* sinfo, void* data) {
  switch (signum) {
    case SIGCHLD:
      if (sinfo->si_pid == pid_chld) {
        write(STDOUT_FILENO, "\nBackground job exited\n", 22);
        pid_chld = 0;
      }
      break;
    case SIGINT:
      // TODO
      break;
    case SIGHUP:
      // TODO
      break;
    default:
      break;
  }
}

void printDir() {
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  printf("pingshell# %s ", cwd);
}

int main(int argc, char *argv[]) {
  char buff[SIZE];
  struct command cmd;
  struct sigaction sa;
  sa.sa_sigaction = mon_handler;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGUSR1);
  sigaddset(&sa.sa_mask, SIGTERM);
  sa.sa_flags = SA_RESTART | SA_SIGINFO;

  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("setting up SIGCHLD");
    return 1;
  }
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("setting up SIGINT");
    return 1;
  }
  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    perror("setting up SIGHUP");
    return 1;
  }

  while(1) {
    printDir();
    if (fgets(buff, SIZE, stdin) != NULL) {
      if (bufferToCommand(buff, &cmd) == -1) {
        fprintf(stderr, "cannot read the command\n");
        free_command(cmd);
        continue;
      }
      if (strncmp(cmd.argv[0], "\n", 1) == 0) {
        free_command(cmd);
        continue;
      } else if (strncmp(cmd.argv[0], "exit", 5) == 0) {
        free_command(cmd);
        exitShell();
      } else if (strncmp(cmd.argv[0], "cd", 2) == 0) moveDir(cmd);
      else {
        execCommand(cmd);
      }
      free_command(cmd);
    }
  }

  return 0;
}
