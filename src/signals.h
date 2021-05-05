#ifndef SIGNALS_H
#define SIGNALS_H
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include "jobs.h"

/**
 * Initializes signal reception
 *    SIGTERM: Ignored
 *    SIGQUIT: Ignored
 *    SIGCHLD: Print the end of the job in the background
 *    SIGINT: Redirects the ctrl + c on the job in foreground
 *    SIGHUP: Sends the signal on all its jobs
 *
 * @return 0 on success, -1 on error
 */
int init_handler();

#endif
