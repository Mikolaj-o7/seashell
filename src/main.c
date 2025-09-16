#include "shell.h"

char *history[MAX_HISTORY];
int history_count = 0;

void load_history_from_file() {
  char *home = getenv("HOME");
  if (!home) return;

  char path[1024];
  snprintf(path, sizeof(path), "%s/.seashell_history", home);

  FILE *history_file = fopen(path, "r");
  if (!history_file) return;

  char line[1024];
  while (fgets(line, sizeof(line), history_file) && history_count < MAX_HISTORY) {
    line[strcspn(line, "\n")] = 0;
    history[history_count++] = strdup(line);
  }

  fclose(history_file);
}

void save_command_in_history(char *command) {
  if (history_count < MAX_HISTORY) {
    history[history_count++] = strdup(command);
  }

  char *home = getenv("HOME");
  if (!home) return;

  char path[1024];
  snprintf(path, sizeof(path), "%s/.seashell_history", home);

  FILE *history_file = fopen(path, "a");
  if (!history_file) return;

  fprintf(history_file, "%s\n", command);
  fclose(history_file);
}

void get_current_working_directory(char *cwd, size_t size) {
  if (getcwd(cwd, size) == NULL) {
    perror("getcwd failed");
  }
}

void get_username(char *username, size_t size) {
  char *user = getenv("USER");

  if (user) {
    snprintf(username, size, "%s", user);
  } else {
    snprintf(username, size, "unknown");
  }
}

void print_prompt() {
  char cwd[PATH_MAX];
  get_current_working_directory(cwd, sizeof(cwd));

  char username[64];
  get_username(username, sizeof(username));

  char *home = getenv("HOME");
  if (home && strncmp(cwd, home, strlen(home)) == 0) {
    printf("%s:~%s > ", username, cwd + strlen(home));
  } else {
    printf("%s:%s > ", username, cwd);
  }
}

int main() {
  load_history_from_file();
  char input[MAX_INPUT];
  char *argv[MAX_ARGS];

  for (;;) {
    print_prompt();
    if (!fgets(input, sizeof(input), stdin)) break;

    input[strcspn(input, "\n")] = 0;
    save_command_in_history(input);

    if (strcmp(input, "exit") == 0) break;

    int argc = 0;
    char *token = strtok(input, " ");
    while (token && argc < MAX_ARGS - 1) {
      argv[argc++] = token;
      token = strtok(NULL, " ");
    }
    argv[argc] = NULL;

    if (argc == 0) continue;

    if (strcmp(argv[0], "history") == 0) {
      for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i]);
      }
      continue;
    }

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

  for (int i = 0; i < history_count; i++) {
    free(history[i]);
  }

  return 0;
}

