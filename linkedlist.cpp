#include <iostream>
#include <cstdio>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <malloc.h>
using namespace std;
struct LZ_freeblock				//内存空闲块结构体
{
	int pfn;					//内存块号
	struct LZ_freeblock *next;	//指向下一个内存块
}*free_head;;
//内存空闲块链表头指针
int LZ_Free = 54;				//初始时内存空闲块数
struct LZ_page					//页表结构体
{
	int flag;			 //标志两类页表
	int pn;				 //页号
	int pfn;			 //块号
	int state;
	struct LZ_page *next;//指向下一个页表项的指针
};
struct LZ_work			//作业结构体
{
	char name[30];		//作业名
	int size;			//作业页数
	int getmSIZE;		//分得的内存块数
	struct LZ_work *next;//指向下一个作业
	struct LZ_page *PAGE;//为其分配内存建立的页表首地址
};
void menu_link()
{
	cout << "\t\t\t主存空间的分配与回收" << endl;
	cout << "\t\t\t您可进行如下操作(输入0结束)：" << endl;
	cout << "\t\t\t1.查看内存分配情况" << endl;
	cout << "\t\t\t2.查看作业" << endl;
	cout << "\t\t\t3.添加作业" << endl;
	cout << "\t\t\t4.删除作业" << endl;
	cout << "\t\t\t目前可用内存为：" << LZ_Free << "块" << endl;
}
void init()				//初始化内存空闲块链表
{
	int block[10] = {-1}, j = 0 , temp;
	srand((unsigned)time(NULL));		//初始化随机数种子
	bool isCF = false;
	for (int i = 0; i < 10; i++)		//生成不重复的随机数
	{
		do{
			temp = rand() % 60;
			for (j = 0; j < 10; j++) {
				if (block[j] == temp) {
					isCF = true;
					break;
				}
			}
			if (j == 10)	break;
		} while (isCF);
		block[i] = temp;
		
	}
	struct LZ_freeblock *p = (LZ_freeblock *)malloc(sizeof(LZ_freeblock));
	free_head = p;
	sort(block, block + 10);		//由小到大排序被占用的内存块
	for (int i = 0; i < 10; i++)
	printf("BLOCK%d\n", block[i]);
	int c = 0;
	for (int i = 0; i < 64; i++)	//将内存块用链表的形式连起来
	{
		if (i != block[c])			//如果没占用
		{
			p->pfn = i;				//块号被占
			if (i != 63)			
			{
				p->next = (LZ_freeblock *)malloc(sizeof(LZ_freeblock));
				p = p->next;
			}
			else					//最后链表末尾置为空
				p->next = NULL;
		}
		else						//如果被占用了不连到链表，看下一块
			c++;
	}
}
void show_link()
{
	LZ_freeblock *q = free_head;
	cout << "内存的空闲块的链接情况如下：" << endl;
	if (q == NULL)cout << "NULL" << endl;
	else
	{
		do
		{
			cout << q->pfn << "->";			//输出页表的空闲块号
			q = q->next;
		} while (q->next != NULL);
		cout << q->pfn << "->NULL" << endl;
	}
}
void  recovery_link(LZ_page *a)//回收页表
{
	LZ_page *b;
	if (a->flag == 0)
	{
		int n = 0;
		LZ_freeblock *head_link, *s;
		s = (LZ_freeblock*)malloc(sizeof(LZ_freeblock));
		while (a->next != NULL)
		{
			if (n == 0)
			{
				head_link = s;
			}
			n++;
			s->pfn = a->pfn;
			s->next = (LZ_freeblock*)malloc(sizeof(LZ_freeblock));
			s = s->next;
			b = a->next;
			free(a);
			a = b;
		}
		s->pfn = a->pfn;
		s->next = free_head;
		free_head = head_link;
		free(a);
	}
}
LZ_work *deletework_link(LZ_work *head_link)//删除作业
{
	char name[30];
	LZ_work *a, *b;
	cout << "请输入要删除的作业名：";
	cin >> name;
	if (head_link == NULL)
	{
		cout << "作业链表为空，没有作业可删除！" << endl;
		return NULL;
	}
	if (strcmp(head_link->name, name) == 0)
	{
		recovery_link(head_link->PAGE);
		LZ_Free += head_link->getmSIZE;
		a = head_link->next;
		free(head_link);
		cout << "作业" << name << "删除成功！" << endl;
		return a;
	}
	a = head_link->next;
	b = head_link;
	while (a != NULL)
	{
		if (strcmp(a->name, name) == 0)
		{
			recovery_link(a->PAGE);
			LZ_Free += a->getmSIZE;
			b->next = a->next;
			free(a);
			cout << "作业" << name << "删除成功！" << endl;
			return head_link;
		}
		b = a;
		a = a->next;
	}
	cout << "您输入的作业名不对，删除作业失败。" << endl;
	return head_link;
}
LZ_page *allocation_link(int size)//分配内存
{
	LZ_freeblock *p = free_head;
	LZ_page *head;
	LZ_page *tmp;
	tmp = (LZ_page *)malloc(sizeof(LZ_page));
	head = tmp;
	if (size <= LZ_Free)			//如果需要分配的内存块数小于空闲块数的话
	{
		int n = 0;
		do
		{
			tmp->flag = 0;
			tmp->pn = n;
			tmp->pfn = p->pfn;		//把块号赋给对应的作业
			tmp->next = NULL;
			n++;
			if (n == size)			//分配结束
				break;
			else
			{
				p = p->next;		//移动指针到下一个空闲块
				tmp->next = (LZ_page *)malloc(sizeof(LZ_page));
				tmp = tmp->next;
			}
		} while (1);

		p = free_head;
		LZ_freeblock *q;
		for (int i = 0; i < size; i++)//释放掉已分配的块的链表
		{
			q = p->next;
			free(p);
			p = q;
		}
		free_head = p;
	}
	else
	{
		cout << "作业大小超过当前内存空闲块数，无法添加作业！" << endl;
	}
	return head;
}
LZ_work *insertwork_link(LZ_work *head_link, LZ_work *b)//载入作业，同位图法
{
	LZ_work *tmp;
	if (head_link == NULL)
	{
		head_link = b;
		b->next = NULL;
	}
	else
	{
		tmp = head_link;
		while (tmp->next != NULL)
		{
			tmp = tmp->next;
		}
		tmp->next = b;
	}
	return head_link;
}
LZ_work *inputwork_link(LZ_work *head_link)//输入作业信息
{
	LZ_work *a;
	char name[30];
	int size;
	cout << "请输入作业名：";
	cin >> name;
	cout << "请输入作业所占内存大小：";
	cin >> size;
	if (size > LZ_Free)
	{
		cout << "内存块数已不足以存放该作业，无法添加作业！" << endl;
		return head_link;
	}
	else
	{
		a = (LZ_work *)malloc(sizeof(LZ_work));
		strcpy(a->name, name);
		a->size = size;
		a->getmSIZE = size;
		a->PAGE = allocation_link(size);//为作业创建页表
		LZ_Free -= size;
		cout << "作业创建成功！" << endl;
		a->next = NULL;
		return insertwork_link(head_link, a);//将作业插入作业链表中
	}
}
void outputpage(LZ_page *a)//输出页表内容
{
	cout << "-----------------------------------------" << endl;
	do
	{
		if (a->flag == 0)
		{
			cout<<" 页号 "<<" 块号 "<<endl;
			cout << setw(4) << a->pn << setw(4) << a->pfn << endl;
		}
		a = a->next;
	} while (a != NULL);
	cout << "-----------------------------------------" << endl;
}

void outputwork_link(LZ_work *head_link)//输出作业信息
{
	if (head_link == NULL)
	{
		cout << "当前没有作业！" << endl;
		return;
	}
	do
	{
		cout << "作业名为：" << head_link->name << "  作业所占内存的大小为：" << head_link->size << "块" << endl;
		cout << "作业的页表如下：" << endl;
		outputpage(head_link->PAGE);
		head_link = head_link->next;
	} while (head_link != NULL);
}
int main()
{
	int n, m;
	LZ_work *head_link;
	head_link = NULL;
	system("cls");
	init();
	menu_link();
	while (cin >> n && n != 0)
	{
		switch (n)
		{
		case 0:
			return 0;
		case 1:
			show_link();		//显示内存的空闲链表
			menu_link();
			break;
		case 2:
			system("cls");
			outputwork_link(head_link);
			menu_link();
			break;
		case 3:
			system("cls");
			head_link = inputwork_link(head_link);
			menu_link();
			break;
		case 4:
			system("cls");
			head_link = deletework_link(head_link);
			menu_link();
			break;
		default:
			break;
		}
	}
	return 0;
}

