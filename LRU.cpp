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

void LRU(int block[], int pSIZE)		//最近最久未使用算法
{
	int memory[work_size + 1] = { 0 };
	memset(memory, -1, sizeof(memory));	//记录，内存块使用情况，初始化为-1
	int flag[work_size + 1] = { 0 };	/*记录页面的访问时间*/
	int i, j, k, m;						//k记录内存中页面个数
	int max = 0;
	int count = 0;
	for (i = 0; i < pSIZE; i++)			//每次来一个随机页面
	{
		max = 0;
		for (j = 0, k = 0; j < mSIZE; j++)//对内存块进行遍历,不断有页进来
		{
			if (memory[j] != block[i])	//与内存块比较：应该是与内存块都不相同的（不管是还有剩余还是都占满的情况）
				k++;
			else {
				flag[j] = i;			//内存中已有，刷新该页的访问时间
			}
				
		}
		if (k == mSIZE)				//j=k>=4，可能满，也可能没满，但是不存在重复的
		{
			int h = 0;
			for (h = 0; h < mSIZE; h++)//那么就要分情况了，先遍历内存块
			{
				if (memory[h] == -1)	//如果还有空就往里放
				{
					memory[h] = block[i];//记录占用该内存块的页号
					flag[h] = i;		//记录占用时间
					break;
				}
			}
			if (h == mSIZE)				//如果内存满了
			{
				count++;				//满了必须置换，记录置换次数
				for (m = 0; m < mSIZE; m++)//此for循环找出时间最久
					if (flag[m] < flag[max]) {
						max = m;		//找出最小的，即为呆的时间最长，最少使用的！！！
					}
						
				int temp = memory[max];
				memory[max] = block[i]; //进行替换
				flag[max] = i;			//记录该页的访问时间
			}
			for (j = 0; j < mSIZE; j++)//更新一下内存情况
				LZ_temp[i][j] = memory[j];
		}
		else							//更新一下内存情况
		{
			for (j = 0; j < mSIZE; j++)
				LZ_temp[i][j] = memory[j];
		}
	}
	print(count, block, pSIZE);
}
int main()
{
	int *block = new int[work_size + 1];	//页面数组
	srand((unsigned)time(NULL));			//初始化随机数种子
	int t, pSIZE = 0;
	//for (int i = 0;; i++)
	//{
	//	t = rand() % 10;
	//	block[pSIZE] = t;					//页面访问顺序随机
	//	pSIZE++;
	//	if (pSIZE == work_size)
	//		break;
	//}
	for (pSIZE = 0; pSIZE < 10; pSIZE++) {
		if(pSIZE==4) block[pSIZE] = 0;
		else block[pSIZE] = pSIZE;
	}

	cout << "页面走势如下：" << endl;
	for (int i = 0; i < work_size; i++)
		cout << block[i] << " ";
	cout << endl;
	LRU(block, pSIZE);
	return 0;
}

