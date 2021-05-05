#include "signals.h"

/**
 * Method that takes care of the actions for the signals 
 *    SIGCHLD: Print the end of the job in the background
 *    SIGINT: Redirects the ctrl + c on the job in foreground
 *    SIGHUP: Sends the signal on all its jobs
 */
void my_handler(int signum, siginfo_t* sinfo, void* data) {
  switch (signum) {
    case SIGCHLD:
      if (sinfo->si_pid == pid_chld) {
        int status;
        if (waitpid(pid_chld, &status, 0) == -1) { perror("waitpid"); }
        if (write(STDOUT_FILENO, "Background job exited\n", 22) == -1) { perror("write"); }
        pid_chld = 0;
      }
      break;
    case SIGINT:
      if (fg_pid> 0) { if (kill(-fg_pid, signum) == -1) { perror("kill"); } }
      break;
    case SIGHUP:
      if (fg_pid> 0) { if (kill(-fg_pid, signum) == -1) { perror("kill"); } }
      if (pid_chld> 0) { if (kill(-pid_chld, signum) == -1) { perror("kill"); }; }
      exit(0);
      break;
    default:
      break;
  }
}

int init_handler(struct sigaction sa, int flags) {
  sa.sa_sigaction = my_handler;
  sa.sa_flags = flags;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    perror("setting up SIGTERM");
    return -1;
  }
  if (sigaction(SIGQUIT, &sa, NULL) == -1) {
    perror("setting up SIGQUIT");
    return -1;
  }
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("setting up SIGCHLD");
    return -1;
  }
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("setting up SIGINT");
    return -1;
  }
  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    perror("setting up SIGHUP");
    return -1;
  }
  return 0;
}
