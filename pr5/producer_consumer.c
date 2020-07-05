#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

/*buffer*/
typedef int buffer_item;
#define BUFFER_SIZE 5
buffer_item buffer[BUFFER_SIZE];
int head = 0;
int tail = 0;
int buffer_num = 0;
sem_t empty, full, race;

int insert_item(buffer_item item)
{
	if (buffer_num == BUFFER_SIZE)
		return -1;
	buffer[tail] = item;
	tail = (tail + 1) % BUFFER_SIZE;
	buffer_num++;
	return 0;
}

int remove_item(buffer_item *item)
{
	if (buffer_num == 0)
		return -1;
	*item = buffer[head];
	head = (head + 1) % BUFFER_SIZE;
	buffer_num--;
	return 0;
}

void *producer(void *param)
{
	srand((unsigned)time(NULL));
	buffer_item item;
	while (1)
	{
		int time = rand()%5+1;
		sleep(time);
		item = rand();
		sem_wait(&empty);
		sem_wait(&race);
		if (insert_item(item))
			printf("Buffer is full.\n");
		else
			printf("producer produced %d.\n", item);
		sem_post(&race);
		sem_post(&full);
	}
}

void *consumer(void *param)
{
	buffer_item item;
	srand((unsigned)time(NULL));

	while (1)
	{
		int time = rand()%5+1;
		sleep(time);
		sem_wait(&full);
		sem_wait(&race);
		if (remove_item(&item))
			printf("Buffer is empty.\n");
		else
			printf("consumer consumed %d.\n",item);
		sem_post(&race);
		sem_post(&empty);
	}
}

int main(int argc, char *argv[])
{
	/*Get conmmand line arguments*/
	
	int t, p_num, c_num;
	
	t = atoi(argv[1]);
	p_num = atoi(argv[2]);
	c_num = atoi(argv[3]);

	/*Initialize buffer*/
	sem_init(&race, 0, 1);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	/*Greate producer thread(s) and consumer thread(s)*/
	pthread_t pro_thread[100];
	pthread_t con_thread[100];

	for(int i = 0; i < p_num; i++)
		pthread_create(&pro_thread[i], NULL, producer, NULL);
	for (int i = 0; i < c_num; i++)
		pthread_create(&con_thread[i], NULL, consumer, NULL);

	sleep(t);

	for (int i = 0; i < p_num; i++)
	{
		pthread_cancel(pro_thread[i]);
		pthread_join(pro_thread[i], NULL);
	}

	for (int i = 0; i < c_num; i++)
	{
		pthread_cancel(con_thread[i]);
		pthread_join(con_thread[i], NULL);
	}
	
	return 0;

}