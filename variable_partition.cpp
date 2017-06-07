#include<stdlib.h>
#include<cstdio>
#include <time.h>
#include <iostream>
using namespace std;
#define Free 0                             //空闲状态
#define Busy 1                             //已用状态
#define INVALID -1
#define OK 1
#define TRUE 1
#define FALSE 0
#define clear_period 50                            //完成

#define total_vp 32
#define total_instruction 50000

#define ERROR 0                            //出错
#define MAX_length 50                     //最大内存空间为50KB
int flag;

typedef struct /*页面结构*/
{
	int pn;                     //页号
	int pfn;                    //页面框架号
	int counter;                //计数器
	int time;                   //时间
} p1_type;
typedef struct pfc_struct   /*页面控制结构，调度算法的控制结构*/
{
	int pn;
	int pfn;
	struct pfc_struct *next;
} pfc_type;
pfc_type pfc[total_vp], *freepf_head, *busypf_head, *busypf_tail;
int diseffect, a[total_instruction];                     //a[]为指令序列
int page[total_instruction], offset[total_instruction]; /*地址信息*/
int initialize(int);
int FIFO(int);

typedef struct freeSpace                   //定义一个空闲区说明表结构
{
	long size;                             //分区大小
	long address;                          //分区地址
	int state;                             //状态
} ElemType;
// 线性表的双向链表存储结构
typedef struct DuLNode
{
	ElemType data;
	struct DuLNode *prior;                 //前趋指针
	struct DuLNode *next;                  //后继指针
}
DuLNode, *DuLinkList;
DuLinkList head_Node;                      //头结点
DuLinkList end_Node;                       //尾结点
int alloc(int);                            //内存分配
int free(int);                             //内存回收
int First_fit(int);                        //首次适应算法
int Best_fit(int);                         //最佳适应算法
int Worst_fit(int);                        //最差适应算法
void show();                               //查看分配
int Initblock();                           //开创空间表
int Initblock()                            //开创带头结点的内存空间链表
{
	head_Node = (DuLinkList)malloc(sizeof(DuLNode));
	end_Node = (DuLinkList)malloc(sizeof(DuLNode));
	head_Node->prior = NULL;                //头结点的前驱指针指向空
	head_Node->next = end_Node;             //头结点的后继指针指向尾结点
	end_Node->prior = head_Node;            //尾结点的前驱指针指向头结点
	end_Node->next = NULL;                  //尾结点的后继指针指向空
	end_Node->data.address = 0;             //尾结点的地址是0
	end_Node->data.size = MAX_length;       //空闲分区大小是最大分区
	end_Node->data.state = Free;            //状态是空
	return OK;
}
void show()
{
	int flag = 0;
	cout << "\n主存分配情况:\n";
	cout << "++++++++++++++++++++++++++++++++++++++++++++++\n\n";
	DuLNode *p = head_Node->next;
	cout << "分区号\t起始地址\t分区大小\t状态\n\n";
	while (p)
	{
		cout << "  " << flag++ << "\t";           //分区加1
		cout << "  " << p->data.address << "\t\t";//输出分区的起始地址
		cout << " " << p->data.size << "KB\t\t";
		if (p->data.state == Free)
			cout << "空闲\n\n";
		else
			cout << "已分配\n\n";
		p = p->next;
	}
	cout << "++++++++++++++++++++++++++++++++++++++++++++++\n\n";
}

int main()
{
	int ch;                               //算法选择标记
	cout << "****存储管理算法模拟****\n";
	while (1) {
		cout << "请输入所使用的内存分配算法：\n";
		cout << "(1)首次适应算法\n(2)最佳适应算法\n(3)最差适应算法\n(4)退出\n";
		cin >> ch;
		while (ch<1 || ch>4)
		{
			cout << "输入错误，请重新输入所使用的内存分配算法：\n";
			cin >> ch;
		}
		if (ch == 4) exit(0);
		Initblock();                          //开创空间表
		int choice;                           //操作选择标记
		while (1)
		{
			show();
			cout << "请输入您的操作：";
			cout << "\n1: 分配内存\n2: 回收内存\n0: 退出\n";

			cin >> choice;
			if (choice == 1)
			{
				alloc(ch);                  // 分配内存

			}
			else if (choice == 2)              // 内存回收
			{
				int flag;
				cout << "请输入您要释放的分区号：";
				cin >> flag;
				free(flag);
			}
			else if (choice == 0)
				break;                     //退出
			else                           //输入操作有误
			{
				cout << "输入有误，请重试!" << endl;
				continue;
			}
		}
	}
	return 0;
}

p1_type p1[total_vp]; /*页面线性结构－指令序列需要使用地址*/
int initialize(int total_pf)
{
	int i;
	diseffect = 0;
	for (i = 0; i<total_pf; i++)
	{
		p1[i].pfn = INVALID;
		p1[i].counter = 0;         /*页面控制结构中的访问次数为0*/
		p1[i].time = 0;           /*访问的时间*/
	}
	for (i = 0; i<(total_pf / 2 - 1); i++)    /*建立pfc[i-1]和pfc[i]之间的链接*/
	{
		pfc[i].next = &pfc[i + 1];
		pfc[i].pfn = i;
	}
	pfc[total_pf / 2 - 1].next = NULL;
	pfc[total_pf / 2 - 1].pfn = total_pf / 2 - 1;
	freepf_head = &pfc[0];         /*空页面队列的头指针为pfc[0]*/
	return 0;
}

//在分配主存函数中确定到底是什么适应
int alloc(int ch)
{
	int ss = 0;
	int need = 0;
	cout << "请输入需要分配的主存大小(单位:KB):";
	cin >> need;
	ss = need;
	if (need<0 || need == 0)
	{
		cout << "请重新输入分配大小!" << endl;
		return ERROR;
	}
	if (ch == 2)                              //选择最佳适应算法
	{
		if (Best_fit(ss) == OK)
			cout << "最佳适应分配成功!" << endl;
		else
			cout << "内存不足，最佳适应分配失败!" << endl;
		return OK;
	}
	if (ch == 3)                              //选择最差适应算法
	{
		if (Worst_fit(ss) == OK)
			cout << "最差适应分配成功!" << endl;
		else
			cout << "内存不足，最差适应分配失败!" << endl;

		return OK;
	}
	else                                   //默认首次适应算法
	{
		if (First_fit(ss) == OK)
			cout << "首次适应分配成功!" << endl;
		else
			cout << "内存不足，首次适应分配失败!" << endl;
		return OK;
	}
	// cout<<"内存分配need="<<need;
}
//首次适应算法
int First_fit(int need)
{
	DuLinkList temp = (DuLinkList)malloc(sizeof(DuLNode));//为申请作业开辟新空间且初始化
	temp->data.size = need;
	temp->data.state = Busy;

	DuLNode *p = head_Node->next;
	while (p)                                             //从前往后开始遍历
	{
		if (p->data.state == Free && p->data.size == need)//现有的空闲块正好等于需要的空间大小，则分配
		{
			p->data.state = Busy;
			return OK;
			break;
		}
		if (p->data.state == Free && p->data.size>need)     //现有的空闲块能满足需求且有剩余
		{
			temp->prior = p->prior;                         //修改双向链表的头尾指针，插入新申请的作业
			temp->next = p;
			temp->data.address = p->data.address;           //分配的起始地址为原先空闲块的起始地址
			p->prior->next = temp;
			p->prior = temp;
			p->data.address = temp->data.address + temp->data.size;//空闲块的起始地址和大小都要改变
			p->data.size -= need;
			return OK;
			break;
		}
		p = p->next;
	}
	return ERROR;
}
//最佳适应算法
int Best_fit(int need)
{
	int ch;													//记录最小剩余空间
	DuLinkList temp = (DuLinkList)malloc(sizeof(DuLNode));
	temp->data.size = need;
	temp->data.state = Busy;
	DuLNode *p = head_Node->next;
	DuLNode *q = NULL;										//记录最佳插入位置
	while (p)												//遍历链表									
	{
		if (p->data.state == Free && (p->data.size >= need))//首先必须是空闲块且大于需要的内存大小
		{
			if (q == NULL)									//如果刚开始q为空时，进行第一轮满足条件的赋值
			{
				q = p;
				ch = p->data.size - need;
			}
			else if (q->data.size > p->data.size)			//找出最小的块出来
			{
				q = p;
				ch = p->data.size - need;
			}
		}
		p = p->next;
	}
	if (q == NULL) return ERROR;                           //没有找到空闲块
	else if (q->data.size == need)						   //找出的最小快如果恰好是需要分配的大小	
	{
		q->data.state = Busy;							   //改变状态	
		return OK;
	}
	else												   //插入内存块设置信息并改变内存块的状态	
	{
		temp->prior = q->prior;			
		temp->next = q;
		temp->data.address = q->data.address;
		q->prior->next = temp;
		q->prior = temp;
		q->data.address += need;
		q->data.size = ch;
		return OK;
	}
	return OK;
}
//最差适应算法
int Worst_fit(int need)
{
	int ch;                                             //记录最大剩余空间
	DuLinkList temp = (DuLinkList)malloc(sizeof(DuLNode));
	temp->data.size = need;
	temp->data.state = Busy;
	DuLNode *p = head_Node->next;
	DuLNode *q = NULL;                                   //记录最佳插入位置
	while (p)                                            //初始化最大空间和最佳位置
	{
		if (p->data.state == Free && (p->data.size >= need))
		{
			if (q == NULL)
			{
				q = p;
				ch = p->data.size - need;
			}
			else if (q->data.size < p->data.size)		//找出最大的块出来
			{
				q = p;
				ch = p->data.size - need;
			}
		}
		p = p->next;
	}
	if (q == NULL) return ERROR;                         //没有找到空闲块
	else if (q->data.size == need)
	{
		q->data.state = Busy;
		return OK;
	}
	else												//插入内存块设置信息并改变内存块的状态	
	{
		temp->prior = q->prior;
		temp->next = q;
		temp->data.address = q->data.address;
		q->prior->next = temp;
		q->prior = temp;
		q->data.address += need;
		q->data.size = ch;
		return OK;
	}
	return OK;
}
//主存回收
/*
当进程运行完毕释放内存，系统根据回收区的首址，从空闲区链表中找到相应的插入点，此时可能出现以下4种情况之一
1回收区与插入点的前一个空闲分区F1相邻接，此时将两个分区合并
2回收区与插入点的后一个空闲分区F2相邻接，此时将两个分区合并
3回收区与插入点的前，后两个空闲分区相邻接，此时将三个分区合并
4回收区既不与F1相邻接，又不与F2相邻接，此时应为回收区单独建立一个新表项
*/
int free(int flag)//flag为需要回收的区号
{
	DuLNode *p = head_Node;
	for (int i = 0; i <= flag; i++)
		if (p != NULL)
			p = p->next;
		else
			return ERROR;
	p->data.state = Free;

	if (p->next == end_Node&&p->next->data.state == Free)//与最后的空闲块相连
	{
		p->data.size += p->next->data.size;
		p->next = NULL;
	}
	if (p->prior != head_Node&&p->prior->data.state == Free&&p->next != NULL)//与前面的空闲块相连
	{
		p->prior->data.size += p->data.size;
		p->prior->next = p->next;
		p->next->prior = p->prior;
		p = p->prior;
	}
	if (p->prior != head_Node&&p->prior->data.state == Free&&p->next == NULL)//与前面的空闲块相连并且这个空闲块为最后一个空闲块
	{
		p->prior->data.size += p->data.size;
		p->prior->next = NULL;
		p = p->prior;
	}
	if (p->next != end_Node&&p->next != NULL&&p->next->data.state == Free)//与后面的空闲块相连
	{
		p->data.size += p->next->data.size;
		if (p->next->next != NULL)
			p->next->next->prior = p;
		p->next = p->next->next;
	}
	return OK;
}
