#include <windows.h>

#include "Command.h"
#include "Global.h"

//��� ó�� Ŭ���� �޼ҵ�
//(���� �迭�� char com={"�̵�","���"...}���� ����� ����, ������ ����� �־ ������ �� �ְ� �Ѵ�.

CCommand::CCommand(char* source[], int x, int y, int width, int line, BOOL row, int max)
				//��ɾ� ���� �迭, X,Y��ǥ, ���̰���, � �̻� ǥ�õǸ� ���� �ٲܰ���, ���� or ����, �ִ�ġ (���ۻ����� �߰��� ��!)
{
	comdlg.SetDlg(x, y, width, line, row);
	Init(source,max);
}

void CCommand::Init(char* source[], int max)
{
	count=0;
	this->source=source;
	command_max=max;
	strcpy(commands,"");
	comdlg.MakeSelection(commands);
}

void CCommand::AddComs(int n, ...)	//��� ������ �߰�
{
	va_list ap;
	va_start(ap,n);
	for(int i=0; i<n; ++i)AddCom(va_arg(ap,int));
	va_end(ap);
}

void CCommand::AddCom(int id)	//����� �� �ִ� ��� �߰�
{
	if(count>=command_max)return;	//��ɾ �� ��
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,source[id]);
	comdlg.MakeSelection(commands);
	com_id[count]=id;
	++count;
}

//���ڹ迭�� �̿����� �ʰ� ���� ���� �ֱ�
void CCommand::AddComStr(int id, char* str)
{
	if(count>=command_max)return;	//��ɾ �� ��
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,str);
	comdlg.MakeSelection(commands);
	com_id[count]=id;
	++count;
}

int CCommand::GetComID(int no)
{
	if(no>=0 && no<command_max)return com_id[no];
		else return -1;
}

//����á���� Ȯ��
BOOL CCommand::IsFull()
{
	if(count>=command_max)return TRUE;
		else return FALSE;
}

//�ǽð����� ����(�ٸ� �۾��� ���ķ� ó��)
int CCommand::CommandSelecting()
{
	if(count<1)return -999;		//��� ���� ����� ������ �������� ����

	int selected;
	selected=comdlg.Selecting();

	if(selected>=0)return GetComID(selected);
		else return -1;
}

//���ø� ��(���μ��� ����)
int CCommand::CommandSelect()
{
	int selected;
	while(1)
	{
		if(!ProcessMessage())break;		//������ �޼��� ó��

		selected=comdlg.Selecting();	//��� ����
		if(selected>=0)break;
		else if(selected==-999)return -1;

		jdd->Render();
	}

	return GetComID(selected);
}

//������� TRUE, �� ������ FALSE�� ����
BOOL CCommand::Empty()
{
	if(count==0)return TRUE;
		else return FALSE;
}

int CCommand::GetCount()
{
	return count;
}