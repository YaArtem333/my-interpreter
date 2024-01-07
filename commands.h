#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>

extern int shell_active;

// Прототипы функций команд
int shcmd_pwd(char* cmd, char* params[]);
int shcmd_exit(char* cmd, char* params[]);
int shcmd_cat(char* cmd, char* params[]);
int shcmd_wc(char* cmd, char* params[]);
int shcmd_nl(char* cmd, char* params[]);
int shcmd_link(char* cmd, char* params[]);
int shcmd_grep(char* cmd, char* params[]);
int shcmd_ls(char* cmd, char* params[]);
int shcmd_ps(char* cmd, char* params[]);
int shcmd_cd(char* cmd, char* params[], int nparams);

#endif // COMMANDS_H
