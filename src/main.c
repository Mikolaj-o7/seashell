#include "shell.h"

int main() {
  char input[MAX_INPUT];
  char *argv[MAX_ARGS];

  for (;;) {
    printf("seash > ");
    if (!fgets(input, sizeof(input), stdin)) break;

    input[strcspn(input, "\n")] = 0;

    if (strcmp(input, "exit") == 0)
      break;

    int argc = 0;
    char *token = strtok(input, " ");
    while (token && argc < MAX_ARGS - 1) {
      argv[argc++] = token;
      token = strtok(NULL, " ");
    }
    argv[argc] = NULL;

    if (argc == 0) continue;

    pid_t pid = fork();
    if (pid == 0) {
      execvp(argv[0], argv);
      perror("execvp failed");
      exit(1);
    } else if (pid > 0) {
      wait(NULL);
    } else {
      perror("fork failed");
    }
  }

  return 0;
}

