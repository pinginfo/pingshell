#include "command.h"

void free_command(struct command cmd) {
  free(cmd.argv);
  if (cmd.next_command != NULL) { free_command(*cmd.next_command); }
}

void print_command(struct command cmd) {
  for (int i = 0; i <= cmd.argc; i++) {
    fprintf(stderr, "cmd.argv[%d] = %s\n", i, cmd.argv[i]);
  }
  fprintf(stderr, "output = %s\nbackground = %d\n", cmd.output, cmd.background_task);
  if (cmd.next_command != NULL) { print_command(*cmd.next_command); }
}

int bufferToCommand(char *input, struct command *cmd) {
  my_argv result = malloc(sizeof(void*));
  my_argv result_pipe = NULL;
  char *buffer;
  size_t nb_args = 0;
  size_t nb_args_pipe = 0;
  const char s[2] = " ";

  cmd->output = NULL;
  cmd->next_command = NULL;
  cmd->background_task = 0;

  strtok(input, "\n"); // Remove the new line from fgets()

  buffer = strtok(input, s);
  while (buffer != NULL) {
    if (strncmp(buffer, "|", STRING_MAX_SIZE) == 0) {
      static struct command pipe_cmd;
      pipe_cmd.next_command = NULL;
      pipe_cmd.output = NULL;
      pipe_cmd.background_task = 0;
      result = realloc(result, sizeof(void*) * (nb_args + 1));
      result[nb_args] = NULL;

      buffer = strtok(NULL, s);
      while (buffer != NULL) {
        if (result_pipe == NULL) { result_pipe = malloc(sizeof(void*)); }
        if (strncmp(buffer, ">", STRING_MAX_SIZE) == 0) {
          buffer = strtok(NULL, s);
          pipe_cmd.output = buffer;
          pipe_cmd.argc = (nb_args_pipe - 1);
          pipe_cmd.argv = result_pipe;
          cmd->next_command = &pipe_cmd;
          break;
        } else if (strncmp(buffer, "&", STRING_MAX_SIZE) == 0) {
          pipe_cmd.background_task = 1;
          pipe_cmd.argc = (nb_args_pipe - 1);
          pipe_cmd.argv = result_pipe;
          cmd->next_command = &pipe_cmd;
          break;
        } else {
          result_pipe = realloc(result_pipe, sizeof(void*) * (nb_args_pipe + 1));
          result_pipe[nb_args_pipe] = buffer;
          nb_args_pipe++;
        }
        buffer = strtok(NULL, s);
      }
      pipe_cmd.argc = (nb_args_pipe - 1);
      pipe_cmd.argv = result_pipe;
      cmd->next_command = &pipe_cmd;
      break;
    } else if (strncmp(buffer, ">", STRING_MAX_SIZE) == 0) {
      buffer = strtok(NULL, s);
      cmd->output = buffer;
      break;
    } else if (strncmp(buffer, "&", STRING_MAX_SIZE) == 0) {
      cmd->background_task = 1;
      break;
    } else {
      result = realloc(result, sizeof(void*) * (nb_args + 1));
      result[nb_args] = buffer;
      nb_args++;
    }
    buffer = strtok(NULL, s);
  }

  cmd->argc = (nb_args - 1);
  cmd->argv = result;
  return 0;
}
