#include "jobs.h"

int pid_chld = 0;
int fg_pid = 0;

void exit_failure(char* p1, char* p2, char* cmd, int err) {
  fprintf(stderr, "%s %s %s %s\n", p1, cmd, p2, strerror(err));
  exit(EXIT_FAILURE);
}

int execCommand(struct command cmd) {
  int status;
  if (cmd.next_command == NULL) {
    pid_t pid = fork();
    if (pid == -1) { 
      fprintf(stderr, "fork: %s\n", strerror(errno));
      return -1;
    } else if (pid == 0) {
      if (cmd.background_task) {
        int out = open("/dev/null", O_RDONLY);
        if (out == -1) { exit_failure("pingshell", "", "open:", errno); }
        if (dup2(out, STDIN_FILENO) == -1) { exit_failure("pingshell:", "", "dup2:", errno); }
      }
      if (cmd.output != NULL) {
        int out = open(cmd.output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (out == -1) { exit_failure("pingshell", "", "open:", errno); }
        if (dup2(out, STDOUT_FILENO) == -1) { exit_failure("pingshell:", "", "dup2:", errno); }
        if (close(out) == -1) { exit_failure("pingshell:", "", "close:", errno); }
      }
      if (setpgid(getpid(), getpid()) != 0) { exit_failure("pingshell:", strerror(errno), "setpgid error", errno); }
      if (execvp(cmd.argv[0], cmd.argv) == -1) { exit_failure("pingshell:", cmd.argv[0], "command not found", errno); }
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
      if (dup2(p[1], STDOUT_FILENO) == -1) { exit_failure("pingshell:", "", "dup2:", errno); }
      if (close(p[0]) == -1) { exit_failure("pingshell:", "", "close:", errno); }
      if (close(p[1]) == -1) { exit_failure("pingshell:", "", "close:", errno); }
      if (setpgid(getpid(), getpid()) != 0) { exit_failure("pingshell:", strerror(errno), "setpgid error", errno); }
      if (execvp(cmd.argv[0], cmd.argv) == -1) { exit_failure("pingshell:", cmd.argv[0], "command not found", errno); }
    } else {
      p2 = fork();
      if (p2 == -1) {
        fprintf(stderr, "fork 1 = %s\n", strerror(errno));
        return -1;
      } else if (p2 == 0) {
        if (dup2(p[0], STDIN_FILENO) == -1) { exit_failure("pingshell:", "", "dup2:", errno); }
        if (close(p[1]) == -1) { exit_failure("pingshell:", "", "close:", errno); }
        if (close(p[0]) == -1) { exit_failure("pingshell:", "", "close:", errno); }
        if (cmd.next_command->output != NULL) {
          int out = open(cmd.next_command->output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
          if (out == -1) { exit_failure("pingshell", "", "open:", errno); }
          if (dup2(out, STDOUT_FILENO) == -1) { exit_failure("pingshell:", "", "dup2:", errno); }
          if (close(out) == -1) { exit_failure("pingshell:", "", "close:", errno); }
        }
        if (setpgid(getpid(), p1) != 0) { exit_failure("pingshell:", strerror(errno), "setpgid error", errno); }
        if (execvp(cmd.next_command->argv[0], cmd.next_command->argv) == -1) { exit_failure("pingshell:", cmd.next_command->argv[0], "command not found", errno); }
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
      } else { fprintf(stdout, "Foreground job [1] %d exited with code %d\n", p1, status); }
      if (waitpid(p2, &status, 0) == -1) { fprintf(stderr, "waitpid : %s\n", strerror(errno)); }
      else { fprintf(stdout, "Foreground job [2] %d exited with code %d\n", p2, status); }
    }
  }
  return 0;
}
