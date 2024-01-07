
#ifndef UTILS_H
#define UTILS_H
#define BUF_SIZE 1024

#include "commands.h"

// Макросы и прототипы вспомогательных функций
#define SHCMD_EXEC(x) shcmd_##x(params[0], params)
#define SHCMD_EXEC1(x) shcmd_##x(params[0], params, np)
#define IS_CMD(x) strncmp(#x, cmd, strlen(cmd)) == 0
void my_exec(char *cmd);
int exec_conv(char *cmds[], int n, int curr);

#endif // UTILS_H
