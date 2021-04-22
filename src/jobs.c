#include "jobs.h"

int pid_chld;

pid_t execCommand(struct command cmd) {
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
          return -1;
        }
        if (dup2(out, 1) == -1) {
          fprintf(stderr, "dup2: %s\n", strerror(errno)); 
          return -1;
        }
        if (close(out) == -1) {
          fprintf(stderr, "close: %s\n", strerror(errno)); 
          return -1;
        }
        if (close(STDOUT_FILENO) == -1) {
          fprintf(stderr, "close: %s\n", strerror(errno)); 
          return -1;
        }
      } else if (cmd.output != NULL) {
        int out = open(cmd.output, O_WRONLY | O_CREAT | O_TRUNC, 0640);
        if (dup2(out, 1) == -1) {
          fprintf(stderr, "dup2: %s\n", strerror(errno)); 
          return -1;
        }
        if (close(out) == -1) {
          fprintf(stderr, "close: %s\n", strerror(errno)); 
          return -1;
        }
      }
      // tcgetpgrp TODO
      if (setpgid(getpid(), getpid()) != 0) {
        fprintf(stderr, "pingshell: %s: setpgid error\n", strerror(errno));
      }
      if (execvp(cmd.argv[0], cmd.argv) == -1) { 
        fprintf(stderr, "pingshell: %s: command not found\n", cmd.argv[0]);
        return -1;
      }
    } else {
      if (!cmd.background_task) {
        wait(0);
        return 0;
      }
      else {
        pid_chld = pid;
        fprintf(stdout, "[1] %d\n", pid_chld);
      }

      return 0;
    }
  } else {
    int p[2];
    if (pipe(p) == -1) {
      fprintf(stderr, "pipe() : %s\n", strerror(errno));
      return -1;
    }
    pid_t p1, p2;

    p1 = fork();
    p2 = fork();

    if (p1 == -1) {
      fprintf(stderr, "fork 1 = %s\n", strerror(errno));
      return -1;
    } else if (p1 == 0) {
      if (close(STDOUT_FILENO) == -1) { 
        fprintf(stderr, "close: %s\n", strerror(errno)); 
        return -1;
      } 
      if (dup(p[1]) == -1) { 
        fprintf(stderr, "dup: %s\n", strerror(errno)); 
        return -1;
      }
      if (close(p[0]) == -1) { 
        fprintf(stderr, "close: %s\n", strerror(errno)); 
        return -1;
      } 
      if (close(p[1]) == -1) { 
        fprintf(stderr, "close: %s\n", strerror(errno)); 
        return -1;
      } 
      if (execvp(cmd.argv[0], cmd.argv) == -1) {
        fprintf(stderr, "execvp = %s\n", strerror(errno));
        fprintf(stderr, "pingshell: %s: command not found\n",  cmd.argv[0]);
        return -1;
      }
    }


    if (p2 == -1) {
      fprintf(stderr, "fork 1 = %s\n", strerror(errno));
      return -1;
    } else if (p2 == 0) {
      if (close(STDIN_FILENO) == -1) {
        fprintf(stderr, "close: %s\n", strerror(errno)); 
        return -1;
      } 
      if (dup(p[0]) == -1) { 
        fprintf(stderr, "dup: %s\n", strerror(errno)); 
        return -1;
      }
      if (close(p[1]) == -1) { 
        fprintf(stderr, "close: %s\n", strerror(errno)); 
        return -1;
      } 
      if (close(p[0]) == -1) { 
        fprintf(stderr, "close: %s\n", strerror(errno)); 
        return -1;
      } 
      if (execvp(cmd.next_command->argv[0], cmd.next_command->argv) == -1) {
        fprintf(stderr, "execvp = %s\n", strerror(errno));
        fprintf(stderr, "pingshell: %s: command not found\n",  cmd.argv[0]);
        return -1;
      }
    }
    close(p[0]);
    close(p[1]);
    /*
       if (setpgid(p1, p1) != 0) {
       fprintf(stderr, "pingshell: %s: setpgid error\n", strerror(errno));
       }
       if (setpgid(p2, p1) != 0) {
       fprintf(stderr, "pingshell: %s: setpgid error\n", strerror(errno));
       }
       */
    wait(0);
    wait(0);
    return 0;
  }
  return 0;
}

