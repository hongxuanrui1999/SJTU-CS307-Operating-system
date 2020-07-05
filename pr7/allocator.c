/*采用分块方式*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	
	int start;
	int end;
	int type;//1 for used,0 for unused
	char *name;
}Block;

struct node
{
	Block* block;
	struct node* prev;
	struct node* next;
};

struct node *head, *tail;//双链表
int memory_size;//初始化时的内存大小
int num_of_blocks = 0;//链表中块的数量

/*在链表中插入一个块，用于初始化和compaction*/
void insert(struct node*h, struct node*t, struct node*newnode)
{
	//如果h->next=t,说明这是一个空链表
	if (h->next == t)
	{
		h->next = newnode;
		t->prev = newnode;
		newnode->prev = h;
		newnode->next = t;
	}
	//如果不是空链表，则插在t的前面
	else
	{
		t->prev->next = newnode;
		newnode->prev = t->prev;
		newnode->next = t;
		t->prev = newnode;
	}
}

/*将链表中的某块一分为二，用于分配内存*/
void split_block(int size, char *name, struct node *oldnode)
{
	Block* block = oldnode->block;

	//如果本块大小恰好等于要分配的内存大小
	if (block->end - block->start + 1 == size)
	{
		strcpy(block->name, name);
		block->type = 1;
	}
	//否则需要一分为二
	else
	{
		Block* tmp_block = malloc(sizeof(Block));
		struct node* newnode = malloc(sizeof(struct node));
		tmp_block->start = block->start;
		tmp_block->end = block->start + size - 1;
		tmp_block->type = 1;
		tmp_block->name = malloc(10 * sizeof(char));
		strcpy(tmp_block->name, name);
		block->start = block->start + size;//修改原块的起始地址
		newnode->block = tmp_block;
		oldnode->prev->next = newnode;
		newnode->prev = oldnode->prev;
		oldnode->prev = newnode;
		newnode->next = oldnode;
	}
}

void print_status()
{
	struct node*tmp = head->next;
	Block* block;
	while (tmp != tail)
	{
		block = tmp->block;
		printf("Addresses [%d:%d] ", block->start, block->end);
		if (block->type)
		{
			printf("Process %s\n", block->name);
		}
		else
			printf("Unused\n");
		tmp = tmp->next;
	}
}

void command_rq(char *command)
{
	int size;//要分配的内存大小
	char flag;//分配内存的方式
	char*name = malloc(10 * sizeof(char));
	struct node*tmp = head->next;
	Block *block;
	sscanf(command, "%s %d %c", name, &size, &flag);

	if (flag == 'F')//first-fit
	{
		while (tmp != tail)
		{
			block = tmp->block;
			if (block->type == 0 && block->end - block->start + 1 >= size)
			{
				split_block(size, name, tmp);
				break;
			}
			tmp = tmp->next;
		}
		if (tmp == tail)
			fprintf(stderr, "No space to allocate.\n");
	}
	else if (flag == 'B')//best-fit
	{
		int min_hole = memory_size;
		struct node*best;
		while (tmp != tail)
		{
			block = tmp->block;
			if (block->type == 0 && block->end - block->start + 1 >= size)
			{
				if (block->end - block->start + 1 <= min_hole)
				{
					min_hole = block->end - block->start + 1;
					best = tmp;
				}
			}
			tmp = tmp->next;
		}
		/*空链表*/
		if ((head->next->block->type==0) && (head->next->block->end-head->next->block->start+1==memory_size))
		{
			split_block(size, name, head->next);
		}
		else
		{
			if (min_hole == memory_size)
				fprintf(stderr, "No space to allocate.\n");
			else
			{
				split_block(size, name, best);
			}
		}
	}
	else if (flag == 'W')//worst-fit
	{
		int max_hole = 0;
		struct node*max_block;
		while (tmp != tail)
		{
			block = tmp->block;
			if (block->type == 0 && block->end - block->start + 1 >= size)
			{
				if (block->end - block->start + 1 > max_hole)
				{
					max_hole = block->end - block->start + 1;
					max_block = tmp;
				}
			}
			tmp = tmp->next;
		}
		if (max_hole == 0)
			fprintf(stderr,"No space to allocate.\n");
		else
			split_block(size, name, max_block);

	}
}

void release_node(struct node*h, struct node*t, struct node*target)
{
	struct node*tmp = h->next;
	while (tmp != tail)
	{
		if (tmp == target)
		{
			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			free(tmp);
			break;
		}
		tmp = tmp->next;
	}
}


/*检查是否可将释放的块和其他块连在一起*/
void merge_block(struct node*release)
{
	struct node*newnode = malloc(sizeof(struct node));
	struct node*newnewnode = malloc(sizeof(struct node));

	struct node* tmp_p = release->prev->prev;
	struct node* tmp_n = release->next;
	Block*block;
	int flag_prev = 0;//用于检测release块是否与前一个块合并了
	int flag_next = 0;//用于检测release是否与后一个块合并了
	if (release->prev != head && release->prev->block->type == 0)
	{
		block = malloc(sizeof(Block));
		block->start = release->prev->block->start;
		block->end = release->block->end;
		block->type = 0;
		newnode->block = block;

		tmp_p = release->prev->prev;
	    tmp_n = release->next;

		release_node(head, tail, release->prev);
		release_node(head, tail, release);
		
		insert(tmp_p, tmp_n, newnode);
		flag_prev = 1;
	}

	if (flag_prev)//release与前一个块合并了，目前的新块是newnode
	{
		if (newnode->next != tail && newnode->next->block->type == 0)
		{
			block = malloc(sizeof(Block));
			block->start = newnode->block->start;
			block->end = release->next->block->end;
			block->type = 0;
			newnewnode->block = block;

			tmp_p = newnode->prev;
			tmp_n = newnode->next->next;

			release_node(head,tail,newnode);
			insert(tmp_p, tmp_n, newnewnode);
		}
	}
	else
	{
		if (release->next != tail && release->next->block->type == 0)
		{
			block = malloc(sizeof(Block));
			block->start = release->block->start;
			block->end = release->next->block->end;
			block->type = 0;
			newnewnode->block = block;

			tmp_p = release->prev->prev;
			tmp_n = release->next;

			release_node(head, tail, release->next);
			release_node(head, tail, release);

			insert(tmp_p, tmp_n, newnewnode);

		}
	}
}

struct node*link(struct node *n1, struct  node *n2)
{
	struct node*newNode = malloc(sizeof(struct node));
	Block*block = malloc(sizeof(Block));
	block->type = 0;
	block->start = n1->block->start;
	block->end = n2->block->end;
	newNode->block = block;

	newNode->prev = n1->prev;
	n1->prev->next = newNode;
	newNode->next = n2->next;
	n2->next->prev = newNode;
	free(n1->block->name); 
	n1->block->name = NULL;
	free(n2->block->name); 
	n2->block->name = NULL;
	free(n1);
	n1 = NULL;
	free(n2); 
	n2 = NULL;
	return newNode;
}


void release(char *name)
{
	struct node *tmp = head->next;
	while (tmp != tail)
	{
		if (tmp->block->type == 1)
		{
			if (strcmp(tmp->block->name, name) == 0)
			{
				tmp->block->type = 0;
				free(tmp->block->name);
				tmp->block->name = NULL;
				if (tmp->prev->block->type == 0)
				{
					struct node*pre = tmp->prev;
					struct node *t = link(pre, tmp);
					tmp = t;
				}
				if (tmp->next->block->type == 0)
				{
					struct node*next = tmp->next;
					struct node *t = link(tmp, next);
					tmp = t;
				}
				return;
			}
		}
		tmp = tmp->next;
	}
}

void command_rl(char *command)
{
	int index = 0;
	char *name = malloc(sizeof(char) * 10);
	for (; index < 50 && command[index] != ' ' && command[index] != '\n'; index++);
	strncpy(name, command, index);
	release(name);

}







void destroy(struct node*h, struct node*t)
{
	struct node*tmp;
	while (h != t)
	{
		tmp = h;
		h = h->next;
		free(tmp);
	}
	free(h);
}






void compaction()
{
	struct node* tmp = head->next;
	struct node*newnode;
	Block*block;
	int index = 0;
	int hole_size = 0;
	int block_num = 0;
	struct node*tmp_next;
	while (tmp != tail)
	{
		if (tmp->block->type == 0)
			hole_size += tmp->block->end - tmp->block->start + 1;
		block_num++;
		tmp = tmp->next;
	}

	tmp = head->next;
	for (int i = 0; i < block_num; i++)
	{
		if (tmp->block->type == 0)
		{
			tmp_next = tmp->next;
			release_node(head, tail, tmp);
		}
		else
		{
			newnode = malloc(sizeof(struct node));
			block = malloc(sizeof(Block));
			block->name = malloc(sizeof(char) * 10);
			block->start = index;
			block->end = index + tmp->block->end - tmp->block->start;
			index = block->end + 1;
			block->type = 1;
			strcpy(block->name, tmp->block->name);
			newnode->block = block;
			insert(head, tail, newnode);
			tmp_next = tmp->next;
			release_node(head, tail, tmp);
		}
		tmp = tmp_next;
	}

	newnode = malloc(sizeof(struct node));
	block = malloc(sizeof(Block));
	block->start = index;
	block->end = index + hole_size - 1;
	block->type = 0;
	newnode->block = block;
	insert(head, tail, newnode);
}

char *preprocess(char *command) {
	char *tmp = malloc(sizeof(char) * 50);
	int i, j = 0, last = 0;
	for (i = 0; i < 50; i++) {
		if (command[i] == '\n')   break;
		if (command[i] != ' ') {
			last = i;
			tmp[j++] = command[i];
		}
		else if (i - last == 1 && j != 0) {
			tmp[j++] = command[i];
		}
	}
	if (tmp[j - 1] == ' ') tmp[j - 1] = '\n';
	else    tmp[j] = '\n';
	free(command);
	command = tmp;
	return command;
}

int main(int argc,char *argv[])
{
	head = malloc(sizeof(struct node*));
	tail = malloc(sizeof(struct node*));
	head->next = tail;
	tail->prev = head;
	memory_size = atoi(argv[1]);
	Block*block = malloc(sizeof(Block));
	struct node*newnode = malloc(sizeof(struct node));
	block->start = 0;
	block->end = memory_size - 1;
	block->type = 0;
	newnode->block = block;
	insert(head, tail, newnode);

	char *command = malloc(50 * sizeof(char));
	while (1)
	{
		printf("allocator<");
		fgets(command, 50, stdin);
		command = preprocess(command);
		if (command[0] == 'X')
		{
			printf("finished.\n");
			break;
		}
		else if (command[0] == 'C')
		{
			compaction();
		}
		else if (strncmp(command, "RQ", 2) == 0)
		{
			command_rq(command + 3);
		}
		else if (strncmp(command, "RL", 2) == 0)
		{
			command_rl(command + 3);
		}
		else if (strncmp(command, "STAT", 2) == 0)
		{
			print_status();
		}
		else
			fprintf(stderr, "error input.\n");
	}
	return 0;
}