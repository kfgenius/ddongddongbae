//���μ��� ������
//2010. 7. 31
//�ڵ���

#pragma once

#include "Process.h"

#define STACK_MAX	100	//���� �Ѱ�

//���ÿ� ���� ���μ��� ������(ID + �Ķ����*4)
struct ProcessStack
{
	int proc_id;
	int ex1;
	int ex2;
	int ex3;
	int ex4;
	char* str1;
	char* str2;
	char* str3;
	char* str4;

	CProcess* p_process;
};

//���μ��� �Ŵ��� Ŭ����
class CProcessManager
{
private:
	//���μ��� ����
	ProcessStack stack[STACK_MAX];
	int stack_count;

protected:
	virtual bool IsEmptyStack();	//������ ������� Ȯ��
	virtual CProcess* CreateProcess(ProcessStack* p_stack);		//���μ��� ����

public:
	virtual CProcess* GetCurrentProcess();	//���� ���μ��� ������ ���

	//���μ��� ���� ����
	virtual void NewProcess(int proc_id, int ex1 = 0, int ex2 = 0, int ex3 = 0, int ex4 = 0);
	virtual void NewProcess(int proc_id, char* str1, char* str2 = NULL, char* str3 = NULL, char* str4 = NULL);
	virtual void StopCurrentProcess();
	virtual void ClearAllProcess();

	//���μ��� ����
	virtual bool Process();

	CProcessManager(void);
	virtual ~CProcessManager(void);
};