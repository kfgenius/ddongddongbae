#include <windows.h>

#include "Command.h"
#include "Global.h"

//��� ó�� Ŭ���� �޼ҵ�
//(���� �迭�� char com={"�̵�","���"...}���� ����� ����, ������ ����� �־ ������ �� �ְ� �Ѵ�.

CCommand::CCommand(char* source[], int x, int y, int width, int line, BOOL row, int max)
//��ɾ� ���� �迭, X,Y��ǥ, ���̰���, � �̻� ǥ�õǸ� ���� �ٲܰ���, ���� or ����, �ִ�ġ (���ۻ����� �߰��� ��!)
{
	com_id_array = NULL;
	com_dlg.SetDlg(x, y, width, line, row);
	Init(source, max);
}

CCommand::~CCommand()
{
	delete [] com_id_array;
	com_id_array = NULL;
}

void CCommand::Init(char* source[], int max)
{
	count = 0;
	this->source = source;
	command_max = max;
	strcpy(commands, "");
	com_dlg.MakeSelection(commands);

	if(com_id_array)
	{
		delete [] com_id_array;
	}

	com_id_array = new int[command_max];
}

void CCommand::AddComs(int n, ...)	//��� ������ �߰�
{
	va_list ap;
	va_start(ap,n);
	for(int i = 0; i < n; i++)
	{
		AddCom(va_arg(ap, int));
	}
	va_end(ap);
}

void CCommand::AddCom(int id)	//����� �� �ִ� ��� �߰�
{
	if(count >= command_max)
	{
		return;	//��ɾ �� ��
	}
	
	if(count > 0)
	{
		strcat(commands,"\\");
	}

	strcat(commands,source[id]);
	com_dlg.MakeSelection(commands);
	com_id_array[count] = id;
	count++;
}

//���ڹ迭�� �̿����� �ʰ� ���� ���� �ֱ�
void CCommand::AddComStr(int id, char* str)
{
	if(count >= command_max)	//��ɾ �� ��
	{
		return;
	}
	
	if(count>0)
	{
		strcat(commands,"\\");
	}

	strcat(commands,str);
	com_dlg.MakeSelection(commands);
	com_id_array[count] = id;
	++count;
}

int CCommand::GetComID(int no)
{
	if(no >= 0 && no < command_max)
	{
		return com_id_array[no];
	}
	else
	{
		return -1;
	}
}

//����á���� Ȯ��
BOOL CCommand::IsFull()
{
	if(count >= command_max)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//�ǽð����� ����(�ٸ� �۾��� ���ķ� ó��)
int CCommand::CommandSelecting()
{
	if(count <= 0)	//��� ���� ����� ������ �������� ����
	{
		return SELECT_CANCEL;
	}

	int selected = com_dlg.Selecting();

	if(selected >= 0)
	{
		return GetComID(selected);
	}
	else
	{
		return -1;
	}
}

//���ø� ��(���μ��� ����)
int CCommand::CommandSelect()
{
	int selected;
	while(TRUE)
	{
		if(!ProcessMessage())	//������ �޼��� ó��
		{
			break;
		}

		selected = com_dlg.Selecting();	//��� ����
		if(selected >= 0)
		{
			break;
		}
		else
		{
			if(selected == SELECT_CANCEL)
			{
				return -1;
			}
		}

		jdd->Render();
	}

	return GetComID(selected);
}

BOOL CCommand::IsEmpty()
{
	if(count == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CCommand::GetCount()
{
	return count;
}