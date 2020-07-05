#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

extern struct node**head;
extern float turnarround_time;
extern float wait_time;
extern float response_time;
extern int task_num;
extern int value;
int flag[50];
int current_time = 0;

void add(char *name, int priority, int burst) {
	Task *task;
	task = malloc(sizeof(Task));
	task->name = malloc(sizeof(char) * 20);
	strcpy(task->name, name);
	task->priority = priority;
	task->burst = burst;
	__sync_fetch_and_add(&value, 1);
	task->tid = value;

	insert(head, task);
}

void add_to_tail(char* name, int priority, int burst, int tid)
{
	Task *task;
	task = malloc(sizeof(Task));
	task->name = malloc(sizeof(char) * 20);
	strcpy(task->name, name);
	task->priority = priority;
	task->burst = burst;
	task->tid = tid;

	insert(head, task);
}

void schedule()
{
	for (int i = 0; i < 50; i++)
	{
		flag[i] = 0;//未被调度过
	}

	struct node *tmp;

	while ((*head) != NULL)
	{
		tmp = *head;
		while (tmp->next != NULL)
			tmp = tmp->next;
		//分为burst<=10和burst>10
		if (tmp->task->burst <= 10)//可以一次执行完毕
		{
			if (flag[tmp->task->tid] == 0)//设置为调度过
			{
				response_time += current_time;
				flag[tmp->task->tid] = 1;
			}
			run(tmp->task, tmp->task->burst);
			wait_time -= tmp->task->burst;
			current_time += tmp->task->burst;
			turnarround_time += current_time;//每个任务都是在0时刻发布的，在此时此刻完成
			delete(head, tmp->task);
		}
		else
		{
			if (flag[tmp->task->tid] == 0)
			{
				response_time += current_time;
				flag[tmp->task->tid] = 1;
			}

			run(tmp->task, 10);
			wait_time -= 10;
			current_time += 10;
			tmp->task->burst -= 10;

			delete(head, tmp->task);
			add_to_tail(tmp->task->name, tmp->task->priority, tmp->task->burst, tmp->task->tid);
		}
	}
	wait_time += turnarround_time;
}
