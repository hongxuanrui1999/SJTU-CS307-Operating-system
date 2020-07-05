#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
	int frame_num;
	int page_num;
	int use;
}TLB;

char phy_memory[256][256];//物理内存
int page[256];//页表
int exist[256];//用于说明page_num=i的页是否存在数据
int vir_address[1000];//存储读入的虚拟地址

int tlb_hit = 0;//tlb命中次数
int miss = 0;//page失效次数
int count = 0;//记录读入的虚拟地址数目
double page_rate = 0;//page失效率
double tlb_rate = 0;//tlb命中率

FILE* file;//虚拟地址文件
FILE* back_store;//存储数据的文件

TLB tlb[16];//tlb

int main(int argc , char*argv[])
{

	char*s = argv[1];
	file = fopen(s, "r");
	back_store = fopen("BACKING_STORE.bin", "r");

	int page_num;
	int frame_num = 0;
	int offset;
	char res;

	for (int i = 0; i < 256; i++)
		exist[i] = 0;
	for (int i = 0; i < 16; i++)
	{
		tlb[i].frame_num = -1;
		tlb[i].page_num = -1;
		tlb[i].use = 10000;
	}

	for (int count = 0; count < 1000; count++)
	{
		fscanf(file, "%d", &vir_address[count]);
		page_num = (vir_address[count] >> 8) & 0xff;
		offset = vir_address[count] % 256;
		int in_tlb = 0;

		if (exist[page_num])//不用从.bin文件中取了，存在于page中
		{
			for (int i = 0; i < 16; i++)//访问TLB
			{
				if (tlb[i].page_num == page_num)//tlb命中
				{
					res = phy_memory[tlb[i].frame_num][offset];
					printf("logical address %d physical address %d result %d\n", vir_address[count], 256 * tlb[i].frame_num + offset, res);
					tlb_hit++;
					tlb[i].use = 0;//更新为0
					in_tlb = 1;
				}
				else
					tlb[i].use++;
			}
			if (in_tlb);
			else//在page中，但是不在tlb中，利用LRU放入tlb中
			{
				res = phy_memory[page[page_num]][offset];
				printf("logical address %d physical address %d result %d\n", vir_address[count], 256 * page[page_num] + offset, res);
				
				int max = 0;
				int max_index = 0;
				for (int j = 0; j < 16; j++)
				{
					if (tlb[j].use > max)
					{
						max_index = j;
						max = tlb[j].use;
					}
				}

				tlb[max_index].frame_num = page[page_num];
				tlb[max_index].page_num = page_num;
				tlb[max_index].use = 0;
			}
			in_tlb = 0;
		}
		else//不在page中
		{
			miss++;
			fseek(back_store, page_num * 256 * sizeof(char), 0);
			fread(phy_memory[frame_num], sizeof(char), 256, back_store);
			page[page_num] = frame_num;
			exist[page_num] = 1;

			res = phy_memory[frame_num][offset];
			printf("logical address %d physical address %d result %d\n", vir_address[count], 256 * frame_num + offset, res);
			frame_num++;

			//放入TLB
			int max = 0;
			int max_index = 0;
			for (int j = 0; j < 16; j++)
			{
				if (tlb[j].use > max)
				{
					max_index = j;
					max = tlb[j].use;
				}
			}

			tlb[max_index].frame_num = page[page_num];
			tlb[max_index].page_num = page_num;
			tlb[max_index].use = 0;

			//将其他tlb中的条目use+1
			for (int j = 0; j < 16; j++)
			{
				if (j == max_index) continue;
				tlb[j].use++;
			}
		}
	}

	tlb_rate = tlb_hit / 1000.0;
	page_rate = miss / 1000.0;
	printf("Page miss rate %f , TLB hit rate %f\n", page_rate, tlb_rate);
}

