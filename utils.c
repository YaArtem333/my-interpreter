
#include "utils.h"
#include <string.h>

// Реализация вспомогательных функций

void my_exec(char *cmd)
{
	char *params[256]; //параметры команды разделенные пробелами
	char *token;
	int np;

	token = strtok(cmd, " ");
	np = 0;
	while( token && np < 255 )
	{
		if (token[0] == '$') // Проверка на переменную окружения
		{
		    char *env_var = getenv(token + 1); // Извлечение значения переменной окружения
		    if (env_var)
		    {
			params[np++] = env_var;
		    }
		    else
		    {
			fprintf(stderr, "Variable %s not found in the environment\n", token + 1);
			return;
		    }
		}
		else
		{
		    params[np++] = token;
		}
		token = strtok(NULL, " ");
	}
	params[np] = NULL;

	// выполнение встроенных команд
	if( IS_CMD(pwd) )
		SHCMD_EXEC(pwd);
	else
	if( IS_CMD(exit) )
		SHCMD_EXEC(exit);
	else
	if( IS_CMD(cat) )
		SHCMD_EXEC(cat);
	else if( IS_CMD(wc) )
		SHCMD_EXEC(wc);
	else if( IS_CMD(nl) )
		SHCMD_EXEC(nl);
	else if( IS_CMD(link) )
		SHCMD_EXEC(link);
	else if( IS_CMD(grep) )
		SHCMD_EXEC(grep);
	else if( IS_CMD(ls) )
		SHCMD_EXEC(ls);
	else if( IS_CMD(ps) )
		SHCMD_EXEC(ps);
	else if( IS_CMD(cd) )
		SHCMD_EXEC1(cd);
	else
	{ // иначе вызов внешней команды
		execvp(params[0], params);
		perror("exec"); // если возникла ошибка при запуске
	}
}

// рекурсивная функция обработки конвейера
// параметры: строка команды, количество команд в конвейере, текущая (начинается с 0 )
int exec_conv(char *cmds[], int n, int curr)
{
	int fd[2],i;
	if( pipe(fd) < 0 )
	{
		printf("Cannot create pipe\n");
		return 1;
	}

	if( n > 1 && curr < n - 2 )
	{ // first n-2 cmds
		if( fork() == 0 )
		{
			dup2(fd[1], 1);
			close(fd[0]);
			close(fd[1]);
			my_exec(cmds[curr]);
			exit(0);
		}
		if( fork() == 0 )
		{
			dup2(fd[0], 0);
			close(fd[0]);
			close(fd[1]);
			exec_conv(cmds,n,++curr);
			exit(0);
		}
	}
	else
	{ // 2 last cmds or if only 1 cmd
		if (n == 1)
		{
			if (strstr(cmds[0], "cd") != NULL)
			{
				// Закрытие файловых дескрипторов пайпа, так как они больше не нужны в данном процессе
				close(fd[0]);
				close(fd[1]);
				//printf("%s \n", cmds[curr]);
				my_exec(cmds[curr]); // Выполнение текущей команды
				return 0; // Возвращаемся без создания новых процессов
			}
		}
		if( n == 1 && strcmp(cmds[0],"exit") == 0 )
		{ // для случая команды exit обходимся без fork, иначе не сможем завершиться
			close(fd[0]);
			close(fd[1]);
			my_exec(cmds[curr]);
			return 0;
		}
		if( fork() == 0 )
		{
			if( n > 1 ) // if more then 1 cmd
				dup2(fd[1], 1);
			close(fd[0]);
			close(fd[1]);
			my_exec(cmds[curr]);
			exit(0);
		}
		if( n > 1 && fork()==0 )
		{
			dup2(fd[0], 0);
			close(fd[0]);
			close(fd[1]);
			my_exec(cmds[curr+1]);
			exit(0);
		}
	}
	close(fd[0]);
	close(fd[1]);

	for( i = 0 ; i < n ; i ++ ) // ожидание завершения всех дочерних процессов
		wait(0);

	return 0;
}
