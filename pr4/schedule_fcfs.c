#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

extern struct node **head;
extern float turnarround_time;
extern float wait_time;
extern float response_time;
extern int value;
int current_time = 0;

void add(char *name, int priority, int burst)
{
	Task *task;
	task = malloc(sizeof(Task));
	task->name = malloc(sizeof(char) * 20);
	strcpy(task->name, name);
	__sync_fetch_and_add(&value, 1);
	task->tid = value;
	task->priority = priority;
	task->burst = burst;
	insert(head, task);
}

void schedule()
{
	struct node *p;
	while ((*head) != NULL)
	{
		p = *head;
		while (p->next != NULL)
			p = p->next;
		run(p->task, p->task->burst);
		wait_time += current_time;
		response_time += current_time;
		current_time += p->task->burst;
		turnarround_time += current_time;
		
		delete(head, p->task);
	}
}
