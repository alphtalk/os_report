#include <iostream>
#include <cstdio>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <malloc.h>
#define work_size 10
#define mSIZE 4
using namespace std;
int LZ_temp[100][10];
void print(int t, int page[], int pSIZE)
{
	int i, j, k, l;
	int flag;
	printf("置换顺序依次为：\n");
	for (k = 0; k <= (pSIZE - 1) / 20; k++)
	{
		for (i = 20 * k; (i < pSIZE) && (i < 20 * (k + 1)); i++)
		{
			if (((i + 1) % 20 == 0) || (((i + 1) % 20) && (i == pSIZE - 1)))
				printf("    %d\n", page[i]);
			else
				printf("    %d", page[i]);
		}
		for (j = 0; j < mSIZE; j++)
		{
			for (i = 20 * k; (i < mSIZE + 20 * k) && (i < pSIZE); i++)
			{
				if (i >= j&&LZ_temp[i][j] != -1)
				{
					if (i == 20 * k)
						printf("   |%d|", LZ_temp[i][j]);
					else
						printf("  |%d|", LZ_temp[i][j]);
				}

				else
				{
					if (i == 20 * k)
						printf("   | |");
					else
						printf("  | |");
				}

			}
			for (i = mSIZE + 20 * k; (i < pSIZE) && (i < 20 * (k + 1)); i++)
			{
				for (flag = 0, l = 0; l < mSIZE; l++)
					if (LZ_temp[i][l] == LZ_temp[i - 1][l])
						flag++;
				if (flag == mSIZE)
					printf("     ");
				else
					printf("  |%d|", LZ_temp[i][j]);
			}
			if (i % 20 == 0)
				continue;
			printf("\n");
		}
	}
	printf("输出结果:         \n");
	printf("缺页次数:%d\t\t", t + mSIZE);
	printf("缺页率:%d/%d\t\t", t + mSIZE, pSIZE);
	printf("置换次数:%d\t\t\n", t);
}

void FIFO(int block[], int pSIZE)//先进先出算法
{
	int i, j, k, m;
	int memory[work_size + 1];
	memset(memory, -1, sizeof(memory));
	int time[work_size + 1] = { 0 };
	int count = 0, max = 0;
	for (i = 0; i < pSIZE; i++)
	{
		max = 0;
		for (j = 0, k = 0; j < mSIZE; j++)
		{
			if (memory[j] != block[i])
				k++;					
			//无else 最本质的不同,若有重复，不进行刷新,则不会记录下替换后比较大的值，故在都找最小值的情况下实现了两种方法
		}
		if (k == mSIZE)
		{
			int h = 0;
			for (h = 0; h < mSIZE; h++)
			{
				if (memory[h] == -1)
				{
					memory[h] = block[i];
					time[h] = i;			//这里用time数组记录占用时间
					break;
				}
			}
			if (h == mSIZE)
			{
				count++;
				for (m = 0; m < mSIZE; m++)
					if (time[m] < time[max])
						max = m;
				int temp = memory[max];
				memory[max] = block[i];
				time[max] = i;
			}
			for (j = 0; j < mSIZE; j++)
				LZ_temp[i][j] = memory[j];
		}
		else
		{
			for (j = 0; j < mSIZE; j++)
				LZ_temp[i][j] = memory[j];
		}
	}
	print(count, block, pSIZE);
}
int main()
{
	int *block = new int[work_size + 1];
	srand((unsigned)time(NULL));
	int t, pSIZE = 0;
	for (int i = 0;; i++)
	{
		t = rand() % 10;
		block[pSIZE] = t;
		pSIZE++;
		if (pSIZE == work_size)
			break;
	}
	cout << "页面走势如下：" << endl;
	for (int i = 0; i < work_size; i++)
		cout << block[i] << " ";
	cout << endl;
	FIFO(block, pSIZE);
	return 0;
}

