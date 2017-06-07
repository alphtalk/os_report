#include <iostream>
#include <cstdio>
#include <iomanip>
#include <cstring>
#include <malloc.h>
using namespace std;
int bitmap[8][8] = {	//初始化位图,用二维数组保存
	{ 1,1,0,0,1,1,1,0 },
	{ 0,1,0,1,0,1,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 1,0,0,0,0,0,0,1 },
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 }
};
int freecount = 54;		//初始时空闲块数
struct page				//页表结构体
{
	int pn;				//页号
	int pfn;			//块号
	struct page *next;	//指向下一个页表项的指针
};
struct work				//作业结构体
{
	char name[30];		//作业名
	int size;
	struct work *next;	//指向下一个作业
	struct page *PAGE;	//为其分配内存建立的页表首地址，依靠此首地址可得到完整的页表
};
page *allocation(int size)	//就是遍历位示图找到空闲块，改1，记录页号分配到块号的情况
{
	page *head;
	page *tmp;
	int n = 0;
	tmp = (page *)malloc(sizeof(page));
	head = tmp;
	for (int i = 0;; i++)				//遍历位图找到空闲块
	{
		if (bitmap[i / 8][i % 8] == 1)	//如果已使用的话跳过这一块
			continue;
		if (bitmap[i / 8][i % 8] == 0)
		{
			bitmap[i / 8][i % 8] = 1;	//将存储快置为已分配
			tmp->pn = n;				//页号是开始分配的第几页
			tmp->pfn = i;				//所对应的块号是多少
			tmp->next = NULL;			//可能分配完成，将后继暂时置为空
			n++;
		}
		if (n == size)					//分配完成，跳出循环
			break;
		else							//否则进入下一页的分配			
		{
			tmp->next = (page *)malloc(sizeof(page));
			tmp = tmp->next;
		}
	}
	return head;
}
void  recovery(page *a)
{
	page *b;
	while (a->next != NULL)		//依次将作业的页号所对应块号置为0，并释放页表内存
	{
		bitmap[a->pfn / 8][a->pfn % 8] = 0;
		b = a->next;
		free(a);
		a = b;
	}
	bitmap[a->pfn / 8][a->pfn % 8] = 0;
	free(a);
}
void outputpage(page *a)//输出页表内容
{
	cout << "-----------------------------------------" << endl;
	do
	{
		cout << setw(4) << a->pn << setw(4) << a->pfn << endl;	//占四位输出页号和块号
		a = a->next;
	} while (a != NULL);
	cout << "-----------------------------------------" << endl;
}
void outputbit()//输出主存分配位图
{
	cout << "主存分配位图如下：" << endl;
	cout << "-----------------------------------------" << endl;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			cout << setw(4) << bitmap[i][j] << setw(4);
		}
		cout << endl;
	}
	cout << "-----------------------------------------" << endl;
}
work *insertwork(work *head, work *b)//载入作业，即将新作业插入作业链表
{
	work *tmp;
	if (head == NULL)				 //如果刚开始链表为空，则作为第一个作业节点
	{
		head = b;
		b->next = NULL;
	}
	else							 //否则链接到作业链表末尾
	{
		tmp = head;					 //临时节点，以防丢失链表首地址
		while (tmp->next != NULL)
		{
			tmp = tmp->next;
		}
		tmp->next = b;				 //找到最后一个，链到尾部
	}
	return head;
}
work *inputwork(work *head)			 //输入作业信息
{
	work *a;
	char name[30];
	int size;
	cout << "请输入作业名：";
	cin >> name;
	cout << "请输入作业所占内存大小：";
	cin >> size;
	if (size > freecount)					//存储快不够
	{
		cout << "作业所需内存的大小大于空闲内存，无法添加作业。" << endl;
		return head;
	}
	a = (work *)malloc(sizeof(work));   //为新作业赋值
	strcpy(a->name, name);
	a->size = size;
	freecount -= size;					//记得减掉空闲块
	a->PAGE = allocation(size);			//为作业创建页表，即第几页对应第几块
	cout << "作业创建成功！" << endl;
	a->next = NULL;
	return insertwork(head, a);			//将作业插入作业链表中
}
void outputwork(work *head)				//输出作业信息
{
	if (head == NULL)
	{
		cout << "当前没有作业！" << endl;
		return;
	}
	do									//如果有作业的话		
	{
		cout << "作业名为：" << head->name << "  作业所占内存的大小为：" << head->size << "块" << endl;
		cout << "作业的页表如下：" << endl;
		outputpage(head->PAGE);			//调用函数，输出作业的页表
		head = head->next;				
	} while (head != NULL);				//依次遍历每个作业
}
work *deletework(work *head)			//删除作业
{
	char name[30];
	work *a, *b;
	cout << "请输入要删除的作业名：";
	cin >> name;
	if (head == NULL)
	{
		cout << "作业链表为空，没有作业可删除！" << endl;
		return NULL;
	}
	b = a = head;
	while (a != NULL)					//如果匹配上名字了
	{
		if (strcmp(a->name, name) == 0)
		{
			recovery(a->PAGE);			//这里恢复要删除作业占用的位示图，释放其页表内存	
			freecount += a->size;		//空闲块数增多了	
			b = a->next;				//把作业这个节点删除，即指向下一个节点	
			free(a);					//释放这个节点	
			cout << "作业" << name << "删除成功！" << endl;
			return b;
		}
	}
	cout << "您输入的作业名不对，删除作业失败。" << endl;
	return head;
}
void menu()
{
	cout << "主存空间的分配与回收，您可进行如下操作：" << endl;
	cout << "1.查看内存分配情况" << endl;
	cout << "2.查看作业" << endl;
	cout << "3.添加作业" << endl;
	cout << "4.删除作业" << endl;
	cout << "目前可用内存为：" << freecount << "块" << endl;
}
int main()
{
	int n;
	work *head;
	head = NULL;
	menu();						//操作与剩余内存块数
	while (1)
	{
		cin >> n;
		switch (n)
		{
		case 1:
			outputbit();		//查看内存分配情况就是把位图输出一下
			menu();
			break;
		case 2:
			system("cls");		//清屏
			outputwork(head);	//输出作业信息
			menu();
			break;
		case 3:
			system("cls");
			head = inputwork(head);//添加作业
			menu();
			break;
		case 4:
			system("cls");
			head = deletework(head);
			menu();
			break;
		default:
			break;
		}
	}
	return 0;
}