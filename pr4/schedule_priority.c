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

void schedule()
{
	struct node *tmp;
	struct node *highest;
	 
	while ((*head) != NULL)
	{
		tmp = *head;
		highest = *head;
		while (tmp != NULL)
		{
			if (tmp->task->priority > highest->task->priority)
			{
				highest = tmp;
			}
			tmp = tmp->next;
		}
		run(highest->task, highest->task->burst);
		wait_time += current_time;
		response_time += current_time;
		current_time += highest->task->burst;
		turnarround_time += current_time;
		delete(head, highest->task);
	}

}
