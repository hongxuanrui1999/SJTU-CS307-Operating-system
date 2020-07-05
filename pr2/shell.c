#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LINE 80 /*The maximum length conmmand*/
#define COMMAND_BUFSIZE 1024;
int tmp_length;
int whether_wait = 1;
char *history[MAX_LINE];
int cur_history_index = -1;

/*读入user命令行参数*/
char *user_input(void)
{
	tmp_length = COMMAND_BUFSIZE;
	char *tmp;//存储命令的数组
	int index = 0;
	tmp = malloc(sizeof(char) * tmp_length);
	int c;
	
	while (1)
	{
		c = getchar();

		if (c == EOF || c == '\n')
		{
			tmp[index] = '\0';
			return tmp;
		}
		else
		{
			tmp[index] = c;
		}
		index++;
		if (index >= tmp_length)
		{
			tmp_length += COMMAND_BUFSIZE;
			tmp = realloc(tmp, tmp_length);
		}
	}
}

/*拆分命令*/
char** user_input_split(char *command)
{
	tmp_length = MAX_LINE;
	int index = 0;
	char **tmp = malloc(tmp_length * sizeof(char*));
	char *token;
	
	token = strtok(command, " \n\t\r");
	while (token != NULL)
	{
		tmp[index] = token;
		index++;

		if (index >= tmp_length)
		{
			tmp_length += MAX_LINE;
			tmp = realloc(tmp, tmp_length * sizeof(char*));
			
		}

		token = strtok(NULL, " \n\r\t");
	}

	if (index > 0 && strcmp(tmp[index - 1], "&") == 0)
	{
		whether_wait = 0;
		tmp[index - 1] = NULL;
	}
	tmp[index] = NULL;
	return tmp;
}
int child_exec(char *command)
{
	char **args = user_input_split(command);

	if (args[0] == NULL)//空指令
	{
		return 1;
	}
	else if (strcmp(args[0], "!!") == 0)
	{
		return history_exec(args);
	}

	//将指令存入history缓冲区中
	cur_history_index = (cur_history_index + 1) % MAX_LINE;
	history[cur_history_index] = malloc(MAX_LINE * sizeof(char));
	char **tmp_command = args;

	while (*tmp_command != NULL)
	{
		strcat(history[cur_history_index], *tmp_command);
		strcat(history[cur_history_index], " ");
		tmp_command++;
	}
	
	if (strcmp(args[0], "exit") == 0)
		return 0;

	return child_work(args);
}
/*执行历史命令*/
int history_exec()
{
	if (cur_history_index == -1 || history[cur_history_index] == NULL)
	{
		fprintf(stderr, "No command in history\n");
		exit(EXIT_FAILURE);
	}

	char **history_command;
	char *tmp = malloc(sizeof(history[cur_history_index]));
	strcat(tmp, history[cur_history_index]);
	printf("%s\n", tmp);
	history_command = user_input_split(tmp);
	return child_work(history_command);
}
/*创建子进程执行命令*/
int child_work(char **args)
{
	pid_t wpid,pid;
	int status;
	int whether_pipe = 0;

	pid = fork();

	if (pid == 0)
	{
		int i = 0;
		int flag = 0;
		int copyfd = 0;

		for (; args[i] != NULL; i++)
		{
			if (strcmp(">", args[i]) == 0)
			{
				args[i] = NULL;
				close(1);//关闭标准输出端
				int fd = open(args[i + 1], O_WRONLY | O_CREAT, 0777);
				copyfd = dup2(fd, 1);

				flag = 1;
				break;
			}
			if (strcmp("<", args[i]) == 0)
			{
				args[i] = NULL;
				int fd = open(args[i + 1], O_RDONLY, 0666);
			    dup2(fd, 0);
				close(fd);
				break;
			}
			if (strcmp("|", args[i]) == 0)
			{
				whether_pipe = 1;
				break;
			}
		}

		if (whether_pipe)//子进程的输出作为父进程的输入
		{
			args[i] = NULL;
			int fd[2];
			int res = pipe(fd);
			/*
			if (res < 0)
			{
				perror("pipe creates failed\n");
				exit(1);
			}
			*/
			pid_t pid2 = fork();
			if (pid2 < 0)
			{
				perror("child process create failed\n");
				exit(1);
			}
			if (pid2 == 0)//子子进程
			{
				close(fd[0]);//关闭读端
				dup2(fd[1], 1);//标准输出定向到写端
				/*if (execvp(args[0], args) < 0)
				{
					perror("execlp failed\n");
					exit(1);
				}*/
				execvp(args[0], args);
				exit(1);
			}
			if (pid2 > 0)//原父进程的子进程
			{
				close(fd[1]);//关闭写端
				dup2(fd[0], 0);//将标准输入重定向到读端
				/*if (execvp(args[i + 1], args) < 0)
				{
					perror("execlp failed");
					exit(1);
				}*/
				execvp(args[i + 1], args);
				exit(1);
			}
		}
		else
		{
			execvp(args[0], args);
			if (flag)
			{
				close(1);
				dup2(copyfd, 1);
			}
		}

		exit(1);
	}
	else if (pid > 0)
	{
		if (whether_wait)
		{
			printf("Parent waits\n");
			wait(NULL);
		}
		else
		{
			printf("Parent doesn't wait\n");
		}
	}
	
	whether_wait = 1;
	return 1;
}



int main(void)
{
	int should_run = 1;
	char *command;

	while (should_run)
	{
		printf("osh>");
		fflush(stdout);//更新输出缓冲区
		command = user_input();
		should_run = child_exec(command);
		free(command);
	}
	return EXIT_SUCCESS;
}