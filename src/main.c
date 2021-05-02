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
        int status;
        waitpid(pid_chld, &status, 0);
        write(STDOUT_FILENO, "Background job exited\n", 22);
        pid_chld = 0;
      }
      break;
    case SIGINT:
      if (fg_pid> 0) { kill(-fg_pid, signum); }
      break;
    case SIGHUP:
      if (fg_pid> 0) { kill(-fg_pid, signum); }
      if (pid_chld> 0) { kill(-pid_chld, signum); }
      exit(0);
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
  sa.sa_flags = SA_RESTART | SA_SIGINFO;

  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    perror("setting up SIGTERM");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGQUIT, &sa, NULL) == -1) {
    perror("setting up SIGQUIT");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("setting up SIGCHLD");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("setting up SIGINT");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    perror("setting up SIGHUP");
    exit(EXIT_FAILURE);
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
}
