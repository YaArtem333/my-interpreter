#include "commands.h"
#include "utils.h"
#include <string.h>

// Реализация встроенных команд

int shcmd_pwd(char *cmd, char *params[]) {
  printf("%s\n", getenv("PWD"));
  return 0;
}

int shcmd_exit(char *cmd, char *params[]) {
  shell_active = 0;
  printf("Bye, bye!\n");
  return 0;
}

int shcmd_cat(char *cmd, char *params[]) {
  FILE *file = fopen(params[1], "r");
  char ch;
  while ((ch = fgetc(file)) != EOF)
    printf("%c", ch);
  printf("\n\n-----------------------------");
  printf("\nThis is my cat command output\n\n");
  return 0;
}

int shcmd_wc(char *cmd, char *params[]) {
  FILE *file = fopen(params[1], "r");
  long long chars = 0, words = 0, lines = 0;
  char ch, last_char = ' ';
  while ((ch = fgetc(file)) != EOF) {
    chars++;

    if (ch == '\n')
      lines++;
    if (ch == ' ' || ch == '\n') {
      if (last_char != ' ' && last_char != '\n')
        words++;
    }
    last_char = ch;
  }
  printf(" Lines: %lld\n Words: %lld\n Characters: %lld\n", lines, words,
         chars);
  return 0;
}

int shcmd_nl(char *cmd, char *params[]) {
  FILE *file = fopen(params[1], "r");
  long long lines = 0;
  char ch;
  while ((ch = fgetc(file)) != EOF) {
    if (ch == '\n')
      lines++;
  }
  printf("Lines: %lld \n", lines);
  return 0;
}

int shcmd_link(char *cmd, char *params[]) {
  if (link(params[1], params[2]) < 0)
    perror("link");
  else {
    printf("link command has done successfully\n");
  }
  return 0;
}

int shcmd_grep(char *cmd, char *params[]) {
  FILE *file = fopen(params[2], "r");
  char line[256];
  while (fgets(line, sizeof(line), file)) {
    if (strstr(line, params[1]))
      printf("%s", line);
  }
  return 0;
}

int shcmd_ls(char *cmd, char *params[]) {
  // Структура для директории
  DIR *dir;

  // Данные для каждой отдельной записи
  struct dirent *entry;

  // Если был предоставлен аргумент, пытаемся открыть его как директорию
  if (params[1]) {
    dir = opendir(params[1]);
    if (!dir) {
      printf("Невозможно открыть '%s'\n", params[1]);
      return 1;
    }
  } else {
    // Иначе открываем текущую директорию
    dir = opendir(".");
  }

  // Если открыть директорию не удалось
  if (dir == NULL) {
    printf("ls: cannot open directory\n");
    return 1;
  }

  // Пока readdir() не вернёт NULL - выводим имя каждой записи
  while ((entry = readdir(dir)) != NULL)
    printf("%s\t", entry->d_name);

  printf("\n");

  // Не забываем закрыть директорию
  closedir(dir);

  return 0;
}

int shcmd_ps(char *cmd, char *params[]) {
  DIR *dir;
  struct dirent *entry;
  FILE *fp;
  char path[BUF_SIZE];
  char cmdline[BUF_SIZE];

  dir = opendir("/proc");
  if (!dir) {
    printf("Cannot open '/proc'\n");
    return 1;
  }

  while ((entry = readdir(dir)) != NULL) {
    // Проверяем, что имя директории состоит только из цифр (т.е. это PID)
    if (strspn(entry->d_name, "0123456789") == strlen(entry->d_name)) {
      snprintf(path, BUF_SIZE, "/proc/%s/cmdline", entry->d_name);
      fp = fopen(path, "r");
      if (fp) {
        if (fgets(cmdline, BUF_SIZE, fp)) {
          printf("PID: %s - Command: %s\n", entry->d_name, cmdline);
        }
        fclose(fp);
      }
    }
  }

  closedir(dir);
  return 0;
}

int shcmd_cd(char *cmd, char *params[], int nparams) {
  if (nparams <= 1) {
    // Если аргумент не указан, перейти в домашний каталог
    const char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
      fprintf(stderr, "cd: HOME not set\n");
      return 1;
    }
    if (chdir(home_dir) != 0) {
      perror("cd");
      return 1;
    }
    // Обновить переменную окружения PWD после смены каталога в текущем
    // процессе
    if (setenv("PWD", home_dir, 1) != 0) {
      perror("setenv");
      return 1;
    }
  } else {
    chdir(params[1]);
    char new_pwd[1024];
    if (getcwd(new_pwd, sizeof(new_pwd)) == NULL) {
      perror("getcwd");
      return 1;
    }
    if (setenv("PWD", new_pwd, 1) != 0) {
      perror("setenv");
      return 1;
    }
  }
  return 0;
}
