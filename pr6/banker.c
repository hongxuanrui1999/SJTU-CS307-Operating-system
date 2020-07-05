#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "assert.h"

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define INPUT_FILEPATH "./input.txt"
#define MAX_LINE_LENGTH 20
#define MAX_CUSTOMERS 10
#define MAX_RESOURCES 5

/*
int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
*/
int *available;
int **maximum;
int **allocation;
int **need;
int num_customers = 0;
int num_resources = 0;

/*比较数组大小,a<=b返回1，a>b返回0*/
int is_smaller_equal(int *a, int *b, int size)
{
	int flag = 1;
	for (int i = 0; i < size; i++)
	{
		if (*(a + i) <= *(b + i));
		else
			flag = 0;
	}
	return flag;
}


/*判断是否为安全状态,安全返回1，不安全返回0*/
int is_safe()
{
	int flag[NUMBER_OF_CUSTOMERS];//标记allocation_i是否全为0,如果为0则置1
	int work[NUMBER_OF_RESOURCES];
	int finished[NUMBER_OF_CUSTOMERS];

	for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		work[i] = available[i];
	}

	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		flag[i] = 1;
		for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			if (allocation[i][j] == 0);
			else flag[i] = 0;
		}
		if (flag[i])
			finished[i] = 1;
		else
			finished[i] = 0;
	}

	while (1)
	{
		int i;
		for (i = 0; i < NUMBER_OF_CUSTOMERS; i++)
		{
			if ((finished[i] == 0) && is_smaller_equal(need[i], work, NUMBER_OF_RESOURCES))
				break;
		}
		if (i == NUMBER_OF_CUSTOMERS)
			break;
		else
		{
			for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
			{
				work[j] += allocation[i][j];
				finished[i] = 1;
			}
		}
	}

	for (int j = 0; j < NUMBER_OF_CUSTOMERS; j++)
	{
		if (finished[j] == 0)
			return 0;
	}
	return 1;
}

int request_resources(int customer_num, int request[])
{
	if (!(is_smaller_equal(request, need[customer_num],NUMBER_OF_RESOURCES)))
		return -1;
	else
	{
		if (!(is_smaller_equal(request, available, NUMBER_OF_RESOURCES)))
			return -1;
		else
		{
			for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
			{
				available[i] -= request[i];
				allocation[customer_num][i] += request[i];
				need[customer_num][i] -= request[i];
			}
			if (is_safe()) return 0;
			else
			{
				for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
				{
					available[i] += request[i];
					allocation[customer_num][i] -= request[i];
					need[customer_num][i] += request[i];
				}
				return -1;
			}
		}
	}
}

void release_resources(int customer_num, int release[])
{
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		need[customer_num][i] += release[i];
		allocation[customer_num][i] -= release[i];
		available[i] += release[i];
	}
}

void init_array() {
	char *buffer;
	int i, j;
	int idx;
	FILE *fp = fopen(INPUT_FILEPATH, "r");
	assert(fp);

	buffer = malloc(sizeof(char) * MAX_LINE_LENGTH);
	
	num_customers = 5;
	num_resources = 4;

	maximum = malloc(sizeof(int *) * num_customers);
	allocation = malloc(sizeof(int *) * num_customers);
	need = malloc(sizeof(int *) * num_customers);
	available = malloc(sizeof(int) * num_resources);

	for (i = 0; i < num_customers; i++) {
		maximum[i] = malloc(sizeof(int) * num_resources);
		allocation[i] = malloc(sizeof(int) * num_resources);
		need[i] = malloc(sizeof(int) * num_resources);
		memset(allocation[i], 0, num_resources);
	}
	memset(available, 0, num_customers);

	i = j = 0;
	while (fgets(buffer, MAX_LINE_LENGTH, fp) != NULL) {
		for (idx = 0; idx < MAX_LINE_LENGTH; idx++) {
			if (buffer[idx] == ',' || buffer[idx] == '\n') {
				need[i][j] = atoi((char *)&buffer[idx - 1]);
				maximum[i][j] = need[i][j];
				j++;
			}
			if (buffer[idx] == '\n') break;
		}
		i++; 
		j = 0;
	}
	fclose(fp);
	printf("Initialization finished.\n");
}


int main(int argc,char *argv[])
{
	init_array();
	available[0] = atoi(argv[1]);
	available[1] = atoi(argv[2]);
	available[2] = atoi(argv[3]);
	available[3] = atoi(argv[4]);
	

	int request[NUMBER_OF_RESOURCES];
	int release[NUMBER_OF_RESOURCES];
	int customer_num;
	char command[20];

	while (1)
	{
		scanf("%s", command);
		if (strcmp(command, "RQ") == 0)
		{
			scanf("%d%d%d%d%d", &customer_num, &request[0], &request[1], &request[2], &request[3]);
			if (request_resources(customer_num, request))
			{
				printf("The request is denied.\n");
			}
			else
				printf("The request is satisfied.\n");
		}
		else if (strcmp(command, "RL") == 0)
		{
			scanf("%d%d%d%d%d", &customer_num, &release[0], &release[1], &release[2], &release[3]);
			release_resources(customer_num, release);
			printf("Release.\n");
		}
		else if (strcmp(command, "*") == 0)
		{
			printf("avalable:\n");
			for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
			{
				printf("%d ", available[i]);
			}
			printf("\n");

			printf("maximum:\n");
			for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
			{
				printf("customer_%d: ",i);
				for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
					printf("%d ", maximum[i][j]);
				printf("\n");
			}

			printf("allocation:\n");
			for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
			{
				printf("customer_%d: ",i);
				for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
					printf("%d ", allocation[i][j]);
				printf("\n");
			}

			printf("need:\n");
			for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
			{
				printf("customer_%d: ",i);
				for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
					printf("%d ", need[i][j]);
				printf("\n");
			}

		}
		else if (strcmp(command, "exit") == 0)
			break;
	}


	return 0;
}