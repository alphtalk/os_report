#include<stdlib.h>
#include<cstdio>
#include <time.h>
#include <iostream>
using namespace std;
#define Free 0                             //����״̬
#define Busy 1                             //����״̬
#define INVALID -1
#define OK 1
#define TRUE 1
#define FALSE 0
#define clear_period 50                            //���

#define total_vp 32
#define total_instruction 50000

#define ERROR 0                            //����
#define MAX_length 50                     //����ڴ�ռ�Ϊ50KB
int flag;

typedef struct /*ҳ��ṹ*/
{
	int pn;                     //ҳ��
	int pfn;                    //ҳ���ܺ�
	int counter;                //������
	int time;                   //ʱ��
} p1_type;
typedef struct pfc_struct   /*ҳ����ƽṹ�������㷨�Ŀ��ƽṹ*/
{
	int pn;
	int pfn;
	struct pfc_struct *next;
} pfc_type;
pfc_type pfc[total_vp], *freepf_head, *busypf_head, *busypf_tail;
int diseffect, a[total_instruction];                     //a[]Ϊָ������
int page[total_instruction], offset[total_instruction]; /*��ַ��Ϣ*/
int initialize(int);
int FIFO(int);

typedef struct freeSpace                   //����һ��������˵����ṹ
{
	long size;                             //������С
	long address;                          //������ַ
	int state;                             //״̬
} ElemType;
// ���Ա��˫������洢�ṹ
typedef struct DuLNode
{
	ElemType data;
	struct DuLNode *prior;                 //ǰ��ָ��
	struct DuLNode *next;                  //���ָ��
}
DuLNode, *DuLinkList;
DuLinkList head_Node;                      //ͷ���
DuLinkList end_Node;                       //β���
int alloc(int);                            //�ڴ����
int free(int);                             //�ڴ����
int First_fit(int);                        //�״���Ӧ�㷨
int Best_fit(int);                         //�����Ӧ�㷨
int Worst_fit(int);                        //�����Ӧ�㷨
void show();                               //�鿴����
int Initblock();                           //�����ռ��
int Initblock()                            //������ͷ�����ڴ�ռ�����
{
	head_Node = (DuLinkList)malloc(sizeof(DuLNode));
	end_Node = (DuLinkList)malloc(sizeof(DuLNode));
	head_Node->prior = NULL;                //ͷ����ǰ��ָ��ָ���
	head_Node->next = end_Node;             //ͷ���ĺ��ָ��ָ��β���
	end_Node->prior = head_Node;            //β����ǰ��ָ��ָ��ͷ���
	end_Node->next = NULL;                  //β���ĺ��ָ��ָ���
	end_Node->data.address = 0;             //β���ĵ�ַ��0
	end_Node->data.size = MAX_length;       //���з�����С��������
	end_Node->data.state = Free;            //״̬�ǿ�
	return OK;
}
void show()
{
	int flag = 0;
	cout << "\n����������:\n";
	cout << "++++++++++++++++++++++++++++++++++++++++++++++\n\n";
	DuLNode *p = head_Node->next;
	cout << "������\t��ʼ��ַ\t������С\t״̬\n\n";
	while (p)
	{
		cout << "  " << flag++ << "\t";           //������1
		cout << "  " << p->data.address << "\t\t";//�����������ʼ��ַ
		cout << " " << p->data.size << "KB\t\t";
		if (p->data.state == Free)
			cout << "����\n\n";
		else
			cout << "�ѷ���\n\n";
		p = p->next;
	}
	cout << "++++++++++++++++++++++++++++++++++++++++++++++\n\n";
}

int main()
{
	int ch;                               //�㷨ѡ����
	cout << "****�洢�����㷨ģ��****\n";
	while (1) {
		cout << "��������ʹ�õ��ڴ�����㷨��\n";
		cout << "(1)�״���Ӧ�㷨\n(2)�����Ӧ�㷨\n(3)�����Ӧ�㷨\n(4)�˳�\n";
		cin >> ch;
		while (ch<1 || ch>4)
		{
			cout << "�������������������ʹ�õ��ڴ�����㷨��\n";
			cin >> ch;
		}
		if (ch == 4) exit(0);
		Initblock();                          //�����ռ��
		int choice;                           //����ѡ����
		while (1)
		{
			show();
			cout << "���������Ĳ�����";
			cout << "\n1: �����ڴ�\n2: �����ڴ�\n0: �˳�\n";

			cin >> choice;
			if (choice == 1)
			{
				alloc(ch);                  // �����ڴ�

			}
			else if (choice == 2)              // �ڴ����
			{
				int flag;
				cout << "��������Ҫ�ͷŵķ����ţ�";
				cin >> flag;
				free(flag);
			}
			else if (choice == 0)
				break;                     //�˳�
			else                           //�����������
			{
				cout << "��������������!" << endl;
				continue;
			}
		}
	}
	return 0;
}

p1_type p1[total_vp]; /*ҳ�����Խṹ��ָ��������Ҫʹ�õ�ַ*/
int initialize(int total_pf)
{
	int i;
	diseffect = 0;
	for (i = 0; i<total_pf; i++)
	{
		p1[i].pfn = INVALID;
		p1[i].counter = 0;         /*ҳ����ƽṹ�еķ��ʴ���Ϊ0*/
		p1[i].time = 0;           /*���ʵ�ʱ��*/
	}
	for (i = 0; i<(total_pf / 2 - 1); i++)    /*����pfc[i-1]��pfc[i]֮�������*/
	{
		pfc[i].next = &pfc[i + 1];
		pfc[i].pfn = i;
	}
	pfc[total_pf / 2 - 1].next = NULL;
	pfc[total_pf / 2 - 1].pfn = total_pf / 2 - 1;
	freepf_head = &pfc[0];         /*��ҳ����е�ͷָ��Ϊpfc[0]*/
	return 0;
}

//�ڷ������溯����ȷ��������ʲô��Ӧ
int alloc(int ch)
{
	int ss = 0;
	int need = 0;
	cout << "��������Ҫ����������С(��λ:KB):";
	cin >> need;
	ss = need;
	if (need<0 || need == 0)
	{
		cout << "��������������С!" << endl;
		return ERROR;
	}
	if (ch == 2)                              //ѡ�������Ӧ�㷨
	{
		if (Best_fit(ss) == OK)
			cout << "�����Ӧ����ɹ�!" << endl;
		else
			cout << "�ڴ治�㣬�����Ӧ����ʧ��!" << endl;
		return OK;
	}
	if (ch == 3)                              //ѡ�������Ӧ�㷨
	{
		if (Worst_fit(ss) == OK)
			cout << "�����Ӧ����ɹ�!" << endl;
		else
			cout << "�ڴ治�㣬�����Ӧ����ʧ��!" << endl;

		return OK;
	}
	else                                   //Ĭ���״���Ӧ�㷨
	{
		if (First_fit(ss) == OK)
			cout << "�״���Ӧ����ɹ�!" << endl;
		else
			cout << "�ڴ治�㣬�״���Ӧ����ʧ��!" << endl;
		return OK;
	}
	// cout<<"�ڴ����need="<<need;
}
//�״���Ӧ�㷨
int First_fit(int need)
{
	DuLinkList temp = (DuLinkList)malloc(sizeof(DuLNode));//Ϊ������ҵ�����¿ռ��ҳ�ʼ��
	temp->data.size = need;
	temp->data.state = Busy;

	DuLNode *p = head_Node->next;
	while (p)                                             //��ǰ����ʼ����
	{
		if (p->data.state == Free && p->data.size == need)//���еĿ��п����õ�����Ҫ�Ŀռ��С�������
		{
			p->data.state = Busy;
			return OK;
			break;
		}
		if (p->data.state == Free && p->data.size>need)     //���еĿ��п���������������ʣ��
		{
			temp->prior = p->prior;                         //�޸�˫�������ͷβָ�룬�������������ҵ
			temp->next = p;
			temp->data.address = p->data.address;           //�������ʼ��ַΪԭ�ȿ��п����ʼ��ַ
			p->prior->next = temp;
			p->prior = temp;
			p->data.address = temp->data.address + temp->data.size;//���п����ʼ��ַ�ʹ�С��Ҫ�ı�
			p->data.size -= need;
			return OK;
			break;
		}
		p = p->next;
	}
	return ERROR;
}
//�����Ӧ�㷨
int Best_fit(int need)
{
	int ch;													//��¼��Сʣ��ռ�
	DuLinkList temp = (DuLinkList)malloc(sizeof(DuLNode));
	temp->data.size = need;
	temp->data.state = Busy;
	DuLNode *p = head_Node->next;
	DuLNode *q = NULL;										//��¼��Ѳ���λ��
	while (p)												//��������									
	{
		if (p->data.state == Free && (p->data.size >= need))//���ȱ����ǿ��п��Ҵ�����Ҫ���ڴ��С
		{
			if (q == NULL)									//����տ�ʼqΪ��ʱ�����е�һ�����������ĸ�ֵ
			{
				q = p;
				ch = p->data.size - need;
			}
			else if (q->data.size > p->data.size)			//�ҳ���С�Ŀ����
			{
				q = p;
				ch = p->data.size - need;
			}
		}
		p = p->next;
	}
	if (q == NULL) return ERROR;                           //û���ҵ����п�
	else if (q->data.size == need)						   //�ҳ�����С�����ǡ������Ҫ����Ĵ�С	
	{
		q->data.state = Busy;							   //�ı�״̬	
		return OK;
	}
	else												   //�����ڴ��������Ϣ���ı��ڴ���״̬	
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
//�����Ӧ�㷨
int Worst_fit(int need)
{
	int ch;                                             //��¼���ʣ��ռ�
	DuLinkList temp = (DuLinkList)malloc(sizeof(DuLNode));
	temp->data.size = need;
	temp->data.state = Busy;
	DuLNode *p = head_Node->next;
	DuLNode *q = NULL;                                   //��¼��Ѳ���λ��
	while (p)                                            //��ʼ�����ռ�����λ��
	{
		if (p->data.state == Free && (p->data.size >= need))
		{
			if (q == NULL)
			{
				q = p;
				ch = p->data.size - need;
			}
			else if (q->data.size < p->data.size)		//�ҳ����Ŀ����
			{
				q = p;
				ch = p->data.size - need;
			}
		}
		p = p->next;
	}
	if (q == NULL) return ERROR;                         //û���ҵ����п�
	else if (q->data.size == need)
	{
		q->data.state = Busy;
		return OK;
	}
	else												//�����ڴ��������Ϣ���ı��ڴ���״̬	
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
//�������
/*
��������������ͷ��ڴ棬ϵͳ���ݻ���������ַ���ӿ������������ҵ���Ӧ�Ĳ���㣬��ʱ���ܳ�������4�����֮һ
1�������������ǰһ�����з���F1���ڽӣ���ʱ�����������ϲ�
2������������ĺ�һ�����з���F2���ڽӣ���ʱ�����������ϲ�
3�������������ǰ�����������з������ڽӣ���ʱ�����������ϲ�
4�������Ȳ���F1���ڽӣ��ֲ���F2���ڽӣ���ʱӦΪ��������������һ���±���
*/
int free(int flag)//flagΪ��Ҫ���յ�����
{
	DuLNode *p = head_Node;
	for (int i = 0; i <= flag; i++)
		if (p != NULL)
			p = p->next;
		else
			return ERROR;
	p->data.state = Free;

	if (p->next == end_Node&&p->next->data.state == Free)//�����Ŀ��п�����
	{
		p->data.size += p->next->data.size;
		p->next = NULL;
	}
	if (p->prior != head_Node&&p->prior->data.state == Free&&p->next != NULL)//��ǰ��Ŀ��п�����
	{
		p->prior->data.size += p->data.size;
		p->prior->next = p->next;
		p->next->prior = p->prior;
		p = p->prior;
	}
	if (p->prior != head_Node&&p->prior->data.state == Free&&p->next == NULL)//��ǰ��Ŀ��п���������������п�Ϊ���һ�����п�
	{
		p->prior->data.size += p->data.size;
		p->prior->next = NULL;
		p = p->prior;
	}
	if (p->next != end_Node&&p->next != NULL&&p->next->data.state == Free)//�����Ŀ��п�����
	{
		p->data.size += p->next->data.size;
		if (p->next->next != NULL)
			p->next->next->prior = p;
		p->next = p->next->next;
	}
	return OK;
}
