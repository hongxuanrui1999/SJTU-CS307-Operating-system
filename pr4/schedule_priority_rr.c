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
int pri[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int pri_num = 0;
struct node **newhead;

void add(char *name, int priority, int burst) {
	Task *task;
	task = malloc(sizeof(Task));
	task->name = malloc(sizeof(char) * 20);
	strcpy(task->name, name);
	task->priority = priority;
	task->burst = burst;
	__sync_fetch_and_add(&value, 1);
	task->tid = value;
	pri[priority]++;
	insert(head, task);
}


void schedule()
{
	struct node* tmp;
	struct node* newnode;
	//将head反过来存，使得T1在头部
	newhead = malloc(sizeof(struct node*));
	*newhead = NULL;
	while ((*head) != NULL)
	{
		newnode = malloc(sizeof(struct node));
		newnode->task = (*head)->task;
		newnode->next = *newhead;
		*newhead = newnode;
		*head = (*head)->next;
	}

	
		for (int i = 10; i > 0; i--)
		{
			if (pri[i] == 0) continue;
			//printf("pri[i]=%d", pri[i]);
			if (pri[i] == 1)
			{
				tmp = *newhead;
				while (tmp != NULL)
				{
					if (tmp->task->priority == i)
					{
						run(tmp->task, tmp->task->burst);
						wait_time -= tmp->task->burst;
						response_time += current_time;
						current_time += tmp->task->burst;
						turnarround_time += current_time;
						delete(newhead, tmp->task);
						pri[i]--;
						break;
					}
					else tmp = tmp->next;
				}
			}
			else
			{
				tmp = *newhead;
				for (int k = 0; k < 50; k++)
				{
					flag[k] = 0;//未被调度过
				}
				while (pri[i] > 0)
				{
					if (tmp->task->priority == i)
					{
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
							delete(newhead, tmp->task);
							pri[i]--;
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
						}
					}
					if (tmp->next == NULL)
						tmp = *newhead;
					else
						tmp = tmp->next;
					
				}
			}
		}
		
	
	wait_time += turnarround_time;
}
