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
	printf("�û�˳������Ϊ��\n");
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
	printf("������:         \n");
	printf("ȱҳ����:%d\t\t", t + mSIZE);
	printf("ȱҳ��:%d/%d\t\t", t + mSIZE, pSIZE);
	printf("�û�����:%d\t\t\n", t);
}

void LRU(int block[], int pSIZE)		//������δʹ���㷨
{
	int memory[work_size + 1] = { 0 };
	memset(memory, -1, sizeof(memory));	//��¼���ڴ��ʹ���������ʼ��Ϊ-1
	int flag[work_size + 1] = { 0 };	/*��¼ҳ��ķ���ʱ��*/
	int i, j, k, m;						//k��¼�ڴ���ҳ�����
	int max = 0;
	int count = 0;
	for (i = 0; i < pSIZE; i++)			//ÿ����һ�����ҳ��
	{
		max = 0;
		for (j = 0, k = 0; j < mSIZE; j++)//���ڴ����б���,������ҳ����
		{
			if (memory[j] != block[i])	//���ڴ��Ƚϣ�Ӧ�������ڴ�鶼����ͬ�ģ������ǻ���ʣ�໹�Ƕ�ռ���������
				k++;
			else {
				flag[j] = i;			//�ڴ������У�ˢ�¸�ҳ�ķ���ʱ��
			}
				
		}
		if (k == mSIZE)				//j=k>=4����������Ҳ����û�������ǲ������ظ���
		{
			int h = 0;
			for (h = 0; h < mSIZE; h++)//��ô��Ҫ������ˣ��ȱ����ڴ��
			{
				if (memory[h] == -1)	//������пվ������
				{
					memory[h] = block[i];//��¼ռ�ø��ڴ���ҳ��
					flag[h] = i;		//��¼ռ��ʱ��
					break;
				}
			}
			if (h == mSIZE)				//����ڴ�����
			{
				count++;				//���˱����û�����¼�û�����
				for (m = 0; m < mSIZE; m++)//��forѭ���ҳ�ʱ�����
					if (flag[m] < flag[max]) {
						max = m;		//�ҳ���С�ģ���Ϊ����ʱ���������ʹ�õģ�����
					}
						
				int temp = memory[max];
				memory[max] = block[i]; //�����滻
				flag[max] = i;			//��¼��ҳ�ķ���ʱ��
			}
			for (j = 0; j < mSIZE; j++)//����һ���ڴ����
				LZ_temp[i][j] = memory[j];
		}
		else							//����һ���ڴ����
		{
			for (j = 0; j < mSIZE; j++)
				LZ_temp[i][j] = memory[j];
		}
	}
	print(count, block, pSIZE);
}
int main()
{
	int *block = new int[work_size + 1];	//ҳ������
	srand((unsigned)time(NULL));			//��ʼ�����������
	int t, pSIZE = 0;
	//for (int i = 0;; i++)
	//{
	//	t = rand() % 10;
	//	block[pSIZE] = t;					//ҳ�����˳�����
	//	pSIZE++;
	//	if (pSIZE == work_size)
	//		break;
	//}
	for (pSIZE = 0; pSIZE < 10; pSIZE++) {
		if(pSIZE==4) block[pSIZE] = 0;
		else block[pSIZE] = pSIZE;
	}

	cout << "ҳ���������£�" << endl;
	for (int i = 0; i < work_size; i++)
		cout << block[i] << " ";
	cout << endl;
	LRU(block, pSIZE);
	return 0;
}

