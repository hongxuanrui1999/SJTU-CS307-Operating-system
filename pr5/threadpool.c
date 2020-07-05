/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
task worktodo;
task queue[QUEUE_SIZE];
int work_head = 0;//pop位置
int work_tail = 0;//push位置
int queue_num = 0;//位于队列中的任务个数
sem_t mutex_notice;//通知线程是否队列中有可执行的任务
sem_t mutex_race;//避免插入时竞争

// the worker bee
pthread_t bee;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
	if (queue_num==QUEUE_SIZE)
	{
		return 1;
	}
	queue[work_tail].function = t.function;
	queue[work_tail].data = t.data;
	work_tail = (work_tail + 1) % QUEUE_SIZE;
	queue_num++;
    return 0;
}

// remove a task from the queue
task dequeue() 
{
	worktodo.function = queue[work_head].function;
	worktodo.data = queue[work_head].data;
	work_head = (work_head + 1) % QUEUE_SIZE;
	queue_num--;
    return worktodo;
}

// the worker thread in the thread pool
void *worker(void *param)
{
	while (1)
	{
		sem_wait(&mutex_notice);
		sem_wait(&mutex_race);
		worktodo = dequeue();
		sem_post(&mutex_race);
		// execute the task
		execute(worktodo.function, worktodo.data);
	}
	pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
	printf("Execute successfully.\n");
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    worktodo.function = somefunction;
    worktodo.data = p;
	sem_wait(&mutex_race);
	int flag;
	flag = enqueue(worktodo);
	sem_post(&mutex_race);
	if (flag)
		printf("submit failure.\n");
	else
	{
		sem_post(&mutex_notice);
		printf("submit successfully.\n");
	}

    return 0;
}

// initialize the thread pool
void pool_init(void)
{
	sem_init(&mutex_notice, 0, 0);
	sem_init(&mutex_race, 0, 1);

	work_head = 0;
	work_tail = 0;
	queue_num = 0;

    pthread_create(&bee,NULL,worker,NULL);
}

// shutdown the thread pool
void pool_shutdown(void)
{
	pthread_cancel(bee);
    pthread_join(bee,NULL);
	printf("exit.\n");
}
