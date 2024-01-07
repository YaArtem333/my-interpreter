
#include "commands.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int shell_active = 1; // требуется для команды exit

// главная функция, цикл ввода строк (разбивка конвейера, запуск команды)
int main() {
  char cmdline[1024];
  char *p, *cmds[256], *token;
  int cmd_cnt;

  while (shell_active) {
    printf("[%s]# ", getenv("PWD"));
    fflush(stdout);

    fgets(cmdline, 1024, stdin);
    if ((p = strstr(cmdline, "\n")) != NULL)
      p[0] = 0;

    token = strtok(cmdline, "|");
    for (cmd_cnt = 0; token && cmd_cnt < 256; cmd_cnt++) {
      cmds[cmd_cnt] = strdup(token);
      token = strtok(NULL, "|");
    }
    cmds[cmd_cnt] = NULL;

    if (cmd_cnt > 0) {
      exec_conv(cmds, cmd_cnt, 0);
    }
  }

  return 0;
}
