#include <iostream>
#include <cstdio>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <malloc.h>
using namespace std;
struct LZ_freeblock				//�ڴ���п�ṹ��
{
	int pfn;					//�ڴ���
	struct LZ_freeblock *next;	//ָ����һ���ڴ��
}*free_head;;
//�ڴ���п�����ͷָ��
int LZ_Free = 54;				//��ʼʱ�ڴ���п���
struct LZ_page					//ҳ��ṹ��
{
	int flag;			 //��־����ҳ��
	int pn;				 //ҳ��
	int pfn;			 //���
	int state;
	struct LZ_page *next;//ָ����һ��ҳ�����ָ��
};
struct LZ_work			//��ҵ�ṹ��
{
	char name[30];		//��ҵ��
	int size;			//��ҵҳ��
	int getmSIZE;		//�ֵõ��ڴ����
	struct LZ_work *next;//ָ����һ����ҵ
	struct LZ_page *PAGE;//Ϊ������ڴ潨����ҳ���׵�ַ
};
void menu_link()
{
	cout << "\t\t\t����ռ�ķ��������" << endl;
	cout << "\t\t\t���ɽ������²���(����0����)��" << endl;
	cout << "\t\t\t1.�鿴�ڴ�������" << endl;
	cout << "\t\t\t2.�鿴��ҵ" << endl;
	cout << "\t\t\t3.�����ҵ" << endl;
	cout << "\t\t\t4.ɾ����ҵ" << endl;
	cout << "\t\t\tĿǰ�����ڴ�Ϊ��" << LZ_Free << "��" << endl;
}
void init()				//��ʼ���ڴ���п�����
{
	int block[10] = {-1}, j = 0 , temp;
	srand((unsigned)time(NULL));		//��ʼ�����������
	bool isCF = false;
	for (int i = 0; i < 10; i++)		//���ɲ��ظ��������
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
	sort(block, block + 10);		//��С��������ռ�õ��ڴ��
	for (int i = 0; i < 10; i++)
	printf("BLOCK%d\n", block[i]);
	int c = 0;
	for (int i = 0; i < 64; i++)	//���ڴ�����������ʽ������
	{
		if (i != block[c])			//���ûռ��
		{
			p->pfn = i;				//��ű�ռ
			if (i != 63)			
			{
				p->next = (LZ_freeblock *)malloc(sizeof(LZ_freeblock));
				p = p->next;
			}
			else					//�������ĩβ��Ϊ��
				p->next = NULL;
		}
		else						//�����ռ���˲�������������һ��
			c++;
	}
}
void show_link()
{
	LZ_freeblock *q = free_head;
	cout << "�ڴ�Ŀ��п������������£�" << endl;
	if (q == NULL)cout << "NULL" << endl;
	else
	{
		do
		{
			cout << q->pfn << "->";			//���ҳ��Ŀ��п��
			q = q->next;
		} while (q->next != NULL);
		cout << q->pfn << "->NULL" << endl;
	}
}
void  recovery_link(LZ_page *a)//����ҳ��
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
LZ_work *deletework_link(LZ_work *head_link)//ɾ����ҵ
{
	char name[30];
	LZ_work *a, *b;
	cout << "������Ҫɾ������ҵ����";
	cin >> name;
	if (head_link == NULL)
	{
		cout << "��ҵ����Ϊ�գ�û����ҵ��ɾ����" << endl;
		return NULL;
	}
	if (strcmp(head_link->name, name) == 0)
	{
		recovery_link(head_link->PAGE);
		LZ_Free += head_link->getmSIZE;
		a = head_link->next;
		free(head_link);
		cout << "��ҵ" << name << "ɾ���ɹ���" << endl;
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
			cout << "��ҵ" << name << "ɾ���ɹ���" << endl;
			return head_link;
		}
		b = a;
		a = a->next;
	}
	cout << "���������ҵ�����ԣ�ɾ����ҵʧ�ܡ�" << endl;
	return head_link;
}
LZ_page *allocation_link(int size)//�����ڴ�
{
	LZ_freeblock *p = free_head;
	LZ_page *head;
	LZ_page *tmp;
	tmp = (LZ_page *)malloc(sizeof(LZ_page));
	head = tmp;
	if (size <= LZ_Free)			//�����Ҫ������ڴ����С�ڿ��п����Ļ�
	{
		int n = 0;
		do
		{
			tmp->flag = 0;
			tmp->pn = n;
			tmp->pfn = p->pfn;		//�ѿ�Ÿ�����Ӧ����ҵ
			tmp->next = NULL;
			n++;
			if (n == size)			//�������
				break;
			else
			{
				p = p->next;		//�ƶ�ָ�뵽��һ�����п�
				tmp->next = (LZ_page *)malloc(sizeof(LZ_page));
				tmp = tmp->next;
			}
		} while (1);

		p = free_head;
		LZ_freeblock *q;
		for (int i = 0; i < size; i++)//�ͷŵ��ѷ���Ŀ������
		{
			q = p->next;
			free(p);
			p = q;
		}
		free_head = p;
	}
	else
	{
		cout << "��ҵ��С������ǰ�ڴ���п������޷������ҵ��" << endl;
	}
	return head;
}
LZ_work *insertwork_link(LZ_work *head_link, LZ_work *b)//������ҵ��ͬλͼ��
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
LZ_work *inputwork_link(LZ_work *head_link)//������ҵ��Ϣ
{
	LZ_work *a;
	char name[30];
	int size;
	cout << "��������ҵ����";
	cin >> name;
	cout << "��������ҵ��ռ�ڴ��С��";
	cin >> size;
	if (size > LZ_Free)
	{
		cout << "�ڴ�����Ѳ����Դ�Ÿ���ҵ���޷������ҵ��" << endl;
		return head_link;
	}
	else
	{
		a = (LZ_work *)malloc(sizeof(LZ_work));
		strcpy(a->name, name);
		a->size = size;
		a->getmSIZE = size;
		a->PAGE = allocation_link(size);//Ϊ��ҵ����ҳ��
		LZ_Free -= size;
		cout << "��ҵ�����ɹ���" << endl;
		a->next = NULL;
		return insertwork_link(head_link, a);//����ҵ������ҵ������
	}
}
void outputpage(LZ_page *a)//���ҳ������
{
	cout << "-----------------------------------------" << endl;
	do
	{
		if (a->flag == 0)
		{
			cout<<" ҳ�� "<<" ��� "<<endl;
			cout << setw(4) << a->pn << setw(4) << a->pfn << endl;
		}
		a = a->next;
	} while (a != NULL);
	cout << "-----------------------------------------" << endl;
}

void outputwork_link(LZ_work *head_link)//�����ҵ��Ϣ
{
	if (head_link == NULL)
	{
		cout << "��ǰû����ҵ��" << endl;
		return;
	}
	do
	{
		cout << "��ҵ��Ϊ��" << head_link->name << "  ��ҵ��ռ�ڴ�Ĵ�СΪ��" << head_link->size << "��" << endl;
		cout << "��ҵ��ҳ�����£�" << endl;
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
			show_link();		//��ʾ�ڴ�Ŀ�������
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

