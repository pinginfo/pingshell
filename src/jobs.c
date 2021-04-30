#include "jobs.h"

int pid_chld = 0;
int fg_pid = 0;

int execCommand(struct command cmd) {
  int status;
  if (cmd.next_command == NULL) {
    pid_t pid = fork();
    if (pid == -1) { 
      fprintf(stderr, "fork: %s\n", strerror(errno));
      return -1;
    }
    else if (pid == 0) {
      if (cmd.background_task) {
        int out = open("/dev/null", O_WRONLY, 0640);
        if (out == -1) {
          fprintf(stderr, "open: %s\n", strerror(errno)); 
          exit(EXIT_FAILURE);
        }
        if (dup2(out, STDIN_FILENO) == -1) {
          fprintf(stderr, "dup2: %s\n", strerror(errno)); 
          exit(EXIT_FAILURE);
        }
      }
      if (cmd.output != NULL) {
        // Vériifer les droits
        int out = open(cmd.output, O_WRONLY | O_CREAT | O_TRUNC, 0640);
        if (dup2(out, STDOUT_FILENO) == -1) {
          fprintf(stderr, "dup2: %s\n", strerror(errno)); 
          exit(EXIT_FAILURE);
        }
        if (close(out) == -1) {
          fprintf(stderr, "close: %s\n", strerror(errno)); 
          exit(EXIT_FAILURE);
        }
      }
      // tcgetpgrp TODO
      if (setpgid(getpid(), getpid()) != 0) {
        fprintf(stderr, "pingshell: %s: setpgid error\n", strerror(errno));
        exit(EXIT_FAILURE);
      }
      if (execvp(cmd.argv[0], cmd.argv) == -1) { 
        fprintf(stderr, "pingshell: %s: command not found\n", cmd.argv[0]);
        exit(EXIT_FAILURE);
      }
    } else {
      if (!cmd.background_task) {
        fg_pid = pid;
        if (waitpid(pid, &status, 0) == -1) {
          fprintf(stderr, "waitpid : %s\n", strerror(errno));
          return -1;
        } else {
          fprintf(stdout, "Foreground job exited with code [%d] %d\n", pid, status);
        }
      }
      else {
        pid_chld = pid;
        fprintf(stdout, "[1] %d\n", pid_chld);
      }
    }
  } else {
    int p[2];
    if (pipe(p) == -1) {
      fprintf(stderr, "pipe() : %s\n", strerror(errno));
      return -1;
    }
    pid_t p1, p2;

    p1 = fork();

    if (p1 == -1) {
      fprintf(stderr, "fork 1 = %s\n", strerror(errno));
      return -1;
    } else if (p1 == 0) {
      if (dup2(p[1], STDOUT_FILENO) == -1) {
        fprintf(stderr, "dup: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
      }
      if (close(p[0]) == -1) {
        fprintf(stderr, "close: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
      }
      if (close(p[1]) == -1) {
        fprintf(stderr, "close: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
      }
      if (setpgid(getpid(), getpid()) != 0) {
        fprintf(stderr, "pingshell: %s: setpgid error\n", strerror(errno));
        exit(EXIT_FAILURE);
      }
      if (execvp(cmd.argv[0], cmd.argv) == -1) {
        fprintf(stderr, "pingshell: %s: command not found, error : %s\n",  cmd.argv[0], strerror(errno));
        exit(EXIT_FAILURE);
      }
    } else {
      p2 = fork();
      if (p2 == -1) {
        fprintf(stderr, "fork 1 = %s\n", strerror(errno));
        return -1;
      } else if (p2 == 0) {
        if (dup2(p[0], STDIN_FILENO) == -1) {
          fprintf(stderr, "dup: %s\n", strerror(errno));
          exit(EXIT_FAILURE);
        }
        if (close(p[1]) == -1) {
          fprintf(stderr, "close: %s\n", strerror(errno));
          exit(EXIT_FAILURE);
        }
        if (close(p[0]) == -1) {
          fprintf(stderr, "close: %s\n", strerror(errno));
          exit(EXIT_FAILURE);
        }
        if (setpgid(getpid(), p1) != 0) {
          fprintf(stderr, "pingshell: %s: setpgid error\n", strerror(errno));
          exit(EXIT_FAILURE);
        }
        if (execvp(cmd.next_command->argv[0], cmd.next_command->argv) == -1) {
          fprintf(stderr, "pingshell: %s: command not found, error : %s\n",  cmd.next_command->argv[0], strerror(errno));
          exit(EXIT_FAILURE);
        }
      }
      if (close(p[0]) == -1) {
        fprintf(stderr, "close: %s\n", strerror(errno));
        return -1;
      }
      if (close(p[1]) == -1) {
        fprintf(stderr, "close: %s\n", strerror(errno));
        return -1;
      }
      fg_pid = p1;
      if (waitpid(p1, &status, 0) == -1) {
        fprintf(stderr, "waitpid : %s\n", strerror(errno));
        return -1;
      } else {
        fprintf(stdout, "Foreground job [1] %d exited with code %d\n", p1, status);
      }
      if (waitpid(p2, &status, 0) == -1) {
        fprintf(stderr, "waitpid : %s\n", strerror(errno));
      } else {
        fprintf(stdout, "Foreground job [2] %d exited with code %d\n", p2, status);
      }
    }
  }
  return 0;
}

