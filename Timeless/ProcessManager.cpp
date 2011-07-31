#include <windows.h>
#include <stdio.h>

#include "ProcessManager.h"

//������
CProcessManager::CProcessManager(void)
{
	stack_count = 0;
}

//�Ҹ���
CProcessManager::~CProcessManager(void)
{
}

//������ ������� Ȯ��
bool CProcessManager::IsEmptyStack()
{
	if(stack_count == 0)
	{
		return TRUE;
	}
	
	return FALSE;
}

//���μ��� ���
CProcess* CProcessManager::GetCurrentProcess()
{
	if(stack_count == 0)
	{
		return NULL;
	}

	return stack[stack_count - 1].p_process;
}

//���μ��� ����
CProcess* CProcessManager::CreateProcess(ProcessStack* p_stack)
{
	//��ӹ��� �� �����ε��ؼ� ���
	//(���� ���� �����Լ����µ� �̱����� ���ؼ� �̷��� ����)

	return NULL;
}

//���ÿ� ���μ��� �߰�
void CProcessManager::NewProcess(int proc_id, int ex1, int ex2, int ex3, int ex4)
{
	//������ ���� ��
	if(stack_count >= STACK_MAX - 1)
	{
		printf("Warning : Stack is full");
		return;	
	}

	//���ÿ� �ֱ�
	stack[stack_count].proc_id = proc_id;
	stack[stack_count].ex1 = ex1;
	stack[stack_count].ex2 = ex2;
	stack[stack_count].ex3 = ex3;
	stack[stack_count].ex4 = ex4;
	stack[stack_count].str1 = NULL;
	stack[stack_count].str2 = NULL;
	stack[stack_count].str3 = NULL;
	stack[stack_count].str4 = NULL;
	stack[stack_count].p_process = NULL;

	++stack_count;
}

void CProcessManager::NewProcess(int proc_id, char* str1, char* str2, char* str3, char* str4)
{
	//������ ���� ��
	if(stack_count >= STACK_MAX - 1)
	{
		printf("Warning : Stack is full");
		return;	
	}

	//���ÿ� �ֱ�
	stack[stack_count].proc_id = proc_id;
	stack[stack_count].str1 = str1;
	stack[stack_count].str2 = str2;
	stack[stack_count].str3 = str3;
	stack[stack_count].str4 = str4;
	stack[stack_count].p_process = NULL;

	++stack_count;
}


//���� ����
void CProcessManager::StopCurrentProcess()
{
	if(stack_count > 0)
	{
		//���� ���μ��� ����
		if(stack[stack_count - 1].p_process)
		{
			delete stack[stack_count - 1].p_process;
			stack[stack_count - 1].p_process = NULL;
		}

		--stack_count;
	}
}

//���� ����
void CProcessManager::ClearAllProcess()
{
	for(int i = stack_count - 1; i >= 0; --i)
	{
		//���μ����� ����
		if(stack[i].p_process)
		{
			delete stack[i].p_process;
			stack[i].p_process = NULL;
		}
	}

	stack_count = 0;
}

//���μ��� ó��
bool CProcessManager::Process()
{
	//�� ���μ��� ����
	if(stack_count > 0 && GetCurrentProcess() == NULL)
	{
		stack[stack_count - 1].p_process = CreateProcess(&stack[stack_count - 1]);
	}
	else if(GetCurrentProcess()->IsStop())
	{
   		StopCurrentProcess();
	}

	return TRUE;
}