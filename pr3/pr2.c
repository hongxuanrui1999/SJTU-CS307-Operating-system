#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int array[100];
int array_after[100];

void QuickSort(int left,int right); 
void merge();
void *runner(void* param);

int main(int argc, char *argv[])
{

	srand((unsigned)time(NULL));
	
	for (int i = 0; i < 100; i++)
	{
		array[i] = rand() % 100;
	}
	printf("Unsorted array: \n");
	for (int i = 0; i < 100; i++)
		printf("%d ", array[i]);

	printf("\n \n");

	char *th1 = "one";
	char *th2 = "two";
	char *th3 = "three";

	pthread_t tid1; 
	pthread_attr_t attr1;

	pthread_t tid2;
	pthread_attr_t attr2;

	pthread_t tid3;
	pthread_attr_t attr3;


	pthread_attr_init(&attr1);
	pthread_create(&tid1, &attr1, runner, th1);

	pthread_attr_init(&attr2);
	pthread_create(&tid2, &attr2, runner, th2);
	//pthread_join(tid2, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	pthread_attr_init(&attr3);
	pthread_create(&tid3, &attr3, runner, th3);
	//pthread_join(tid3, NULL);

	pthread_join(tid3, NULL);

	printf("Sorted Array:\n");
	int j;
	
	for (j = 0; j < 100; j++) {
		if (j == 99) {
			printf("%d\n", array_after[j]);
		}
		else {
			printf("%d ", array_after[j]);
		}
	}
	
	
	
}

void* runner(void *param)
{
	if (strcmp(param, "one") == 0)
	{
		QuickSort(0, 49);
	}
	else if (strcmp(param, "two") == 0)
	{
		QuickSort(50, 99);
	}
	else if(strcmp(param,"three")==0)
	{
		merge();
	}

	pthread_exit(0);
}


int partition(int left, int right)
{
	int pivot = array[left];
	int l = left;
	int r = right;
	int middle;
	int tmp;
	while (l < r)
	{
		while (l <= right && array[l] <= pivot)
		{
			l++;
		}
		while (r >= left && array[r] > pivot)
		{
			r--;
		}
		if (l < r)
		{
			tmp = array[l];
			array[l] = array[r];
			array[r] = tmp;
		}
	}
	middle = r;
	tmp = array[left];
	array[left] = array[middle];
	array[middle] = tmp;
	return middle;
}

void QuickSort(int left,int right) 
{
	int mid;
	if (left < right)
	{
		mid = partition(left, right);
		//int mid = (left + right) / 2;
		QuickSort(left, mid-1);
		QuickSort(mid+1, right);
	}
}

void merge()
{
	int i = 0;
	int j = 50;
	int index = 0;

	while (i <= 49 && j <= 99)
	{
		if (array[i] <= array[j])
		{
			array_after[index] = array[i];
			//printf("%d,%d,%d\n", array[i], array[j],array_after[index]);
			index++;
			i++;
		}
		else
		{
			array_after[index] = array[j];
			//printf("%d,%d,%d\n", array[j], array[i],array_after[index]);
			index++;
			j++;
		}
	}
	while (j <= 99)
	{
		array_after[index] = array[j];
		j++;
		index++;
	}
	while (i <= 49)
	{
		array_after[index] = array[i];
		i++;
		index++;
	}
	
}