#include <iostream>
#include <cstdio>
#include <iomanip>
#include <cstring>
#include <malloc.h>
using namespace std;
int bitmap[8][8] = {	//��ʼ��λͼ,�ö�ά���鱣��
	{ 1,1,0,0,1,1,1,0 },
	{ 0,1,0,1,0,1,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 1,0,0,0,0,0,0,1 },
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0 }
};
int freecount = 54;		//��ʼʱ���п���
struct page				//ҳ��ṹ��
{
	int pn;				//ҳ��
	int pfn;			//���
	struct page *next;	//ָ����һ��ҳ�����ָ��
};
struct work				//��ҵ�ṹ��
{
	char name[30];		//��ҵ��
	int size;
	struct work *next;	//ָ����һ����ҵ
	struct page *PAGE;	//Ϊ������ڴ潨����ҳ���׵�ַ���������׵�ַ�ɵõ�������ҳ��
};
page *allocation(int size)	//���Ǳ���λʾͼ�ҵ����п飬��1����¼ҳ�ŷ��䵽��ŵ����
{
	page *head;
	page *tmp;
	int n = 0;
	tmp = (page *)malloc(sizeof(page));
	head = tmp;
	for (int i = 0;; i++)				//����λͼ�ҵ����п�
	{
		if (bitmap[i / 8][i % 8] == 1)	//�����ʹ�õĻ�������һ��
			continue;
		if (bitmap[i / 8][i % 8] == 0)
		{
			bitmap[i / 8][i % 8] = 1;	//���洢����Ϊ�ѷ���
			tmp->pn = n;				//ҳ���ǿ�ʼ����ĵڼ�ҳ
			tmp->pfn = i;				//����Ӧ�Ŀ���Ƕ���
			tmp->next = NULL;			//���ܷ�����ɣ��������ʱ��Ϊ��
			n++;
		}
		if (n == size)					//������ɣ�����ѭ��
			break;
		else							//���������һҳ�ķ���			
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
	while (a->next != NULL)		//���ν���ҵ��ҳ������Ӧ�����Ϊ0�����ͷ�ҳ���ڴ�
	{
		bitmap[a->pfn / 8][a->pfn % 8] = 0;
		b = a->next;
		free(a);
		a = b;
	}
	bitmap[a->pfn / 8][a->pfn % 8] = 0;
	free(a);
}
void outputpage(page *a)//���ҳ������
{
	cout << "-----------------------------------------" << endl;
	do
	{
		cout << setw(4) << a->pn << setw(4) << a->pfn << endl;	//ռ��λ���ҳ�źͿ��
		a = a->next;
	} while (a != NULL);
	cout << "-----------------------------------------" << endl;
}
void outputbit()//����������λͼ
{
	cout << "�������λͼ���£�" << endl;
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
work *insertwork(work *head, work *b)//������ҵ����������ҵ������ҵ����
{
	work *tmp;
	if (head == NULL)				 //����տ�ʼ����Ϊ�գ�����Ϊ��һ����ҵ�ڵ�
	{
		head = b;
		b->next = NULL;
	}
	else							 //�������ӵ���ҵ����ĩβ
	{
		tmp = head;					 //��ʱ�ڵ㣬�Է���ʧ�����׵�ַ
		while (tmp->next != NULL)
		{
			tmp = tmp->next;
		}
		tmp->next = b;				 //�ҵ����һ��������β��
	}
	return head;
}
work *inputwork(work *head)			 //������ҵ��Ϣ
{
	work *a;
	char name[30];
	int size;
	cout << "��������ҵ����";
	cin >> name;
	cout << "��������ҵ��ռ�ڴ��С��";
	cin >> size;
	if (size > freecount)					//�洢�첻��
	{
		cout << "��ҵ�����ڴ�Ĵ�С���ڿ����ڴ棬�޷������ҵ��" << endl;
		return head;
	}
	a = (work *)malloc(sizeof(work));   //Ϊ����ҵ��ֵ
	strcpy(a->name, name);
	a->size = size;
	freecount -= size;					//�ǵü������п�
	a->PAGE = allocation(size);			//Ϊ��ҵ����ҳ�����ڼ�ҳ��Ӧ�ڼ���
	cout << "��ҵ�����ɹ���" << endl;
	a->next = NULL;
	return insertwork(head, a);			//����ҵ������ҵ������
}
void outputwork(work *head)				//�����ҵ��Ϣ
{
	if (head == NULL)
	{
		cout << "��ǰû����ҵ��" << endl;
		return;
	}
	do									//�������ҵ�Ļ�		
	{
		cout << "��ҵ��Ϊ��" << head->name << "  ��ҵ��ռ�ڴ�Ĵ�СΪ��" << head->size << "��" << endl;
		cout << "��ҵ��ҳ�����£�" << endl;
		outputpage(head->PAGE);			//���ú����������ҵ��ҳ��
		head = head->next;				
	} while (head != NULL);				//���α���ÿ����ҵ
}
work *deletework(work *head)			//ɾ����ҵ
{
	char name[30];
	work *a, *b;
	cout << "������Ҫɾ������ҵ����";
	cin >> name;
	if (head == NULL)
	{
		cout << "��ҵ����Ϊ�գ�û����ҵ��ɾ����" << endl;
		return NULL;
	}
	b = a = head;
	while (a != NULL)					//���ƥ����������
	{
		if (strcmp(a->name, name) == 0)
		{
			recovery(a->PAGE);			//����ָ�Ҫɾ����ҵռ�õ�λʾͼ���ͷ���ҳ���ڴ�	
			freecount += a->size;		//���п���������	
			b = a->next;				//����ҵ����ڵ�ɾ������ָ����һ���ڵ�	
			free(a);					//�ͷ�����ڵ�	
			cout << "��ҵ" << name << "ɾ���ɹ���" << endl;
			return b;
		}
	}
	cout << "���������ҵ�����ԣ�ɾ����ҵʧ�ܡ�" << endl;
	return head;
}
void menu()
{
	cout << "����ռ�ķ�������գ����ɽ������²�����" << endl;
	cout << "1.�鿴�ڴ�������" << endl;
	cout << "2.�鿴��ҵ" << endl;
	cout << "3.�����ҵ" << endl;
	cout << "4.ɾ����ҵ" << endl;
	cout << "Ŀǰ�����ڴ�Ϊ��" << freecount << "��" << endl;
}
int main()
{
	int n;
	work *head;
	head = NULL;
	menu();						//������ʣ���ڴ����
	while (1)
	{
		cin >> n;
		switch (n)
		{
		case 1:
			outputbit();		//�鿴�ڴ����������ǰ�λͼ���һ��
			menu();
			break;
		case 2:
			system("cls");		//����
			outputwork(head);	//�����ҵ��Ϣ
			menu();
			break;
		case 3:
			system("cls");
			head = inputwork(head);//�����ҵ
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