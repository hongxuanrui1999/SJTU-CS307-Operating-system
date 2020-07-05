/**
* Driver.c
*
* Schedule is in the format
*
*  [name] [priority] [CPU burst]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"

#define SIZE    100

struct node **head;
float turnarround_time = 0;
float wait_time = 0;
float response_time = 0;
int task_num = 0;
int value = 0;
int main(int argc, char *argv[])
{
	FILE *in;
	char *temp;
	char task[SIZE];

	char *name;
	int priority;
	int burst;

	head = malloc(sizeof(struct node *));
	in = fopen(argv[1], "r");

	while (fgets(task, SIZE, in) != NULL) {
		task_num++;
		temp = strdup(task);
		name = strsep(&temp, ",");
		priority = atoi(strsep(&temp, ","));
		burst = atoi(strsep(&temp, ","));
		add(name, priority, burst);

		free(temp);
	}

	fclose(in);

	// invoke the scheduler
	schedule();
	printf("Average turnaround time = %f\n", turnarround_time / task_num);
	printf("Average wait time = %f\n", wait_time / task_num);
	printf("Average response time = %f\n", response_time / task_num);

	return 0;
}
