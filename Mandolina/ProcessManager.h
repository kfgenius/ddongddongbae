//프로세스 관리자
//2010. 7. 31
//박동흥

#pragma once

#include "Process.h"

#define STACK_MAX	100	//스택 한계

//스택에 들어가는 프로세스 데이터(ID + 파라미터*4)
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

//프로세스 매니저 클래스
class CProcessManager
{
private:
	//프로세스 스택
	ProcessStack stack[STACK_MAX];
	int stack_count;

protected:
	virtual bool IsEmptyStack();	//스택이 비었는지 확인
	virtual CProcess* CreateProcess(ProcessStack* p_stack);		//프로세스 생성

public:
	virtual CProcess* GetCurrentProcess();	//현재 프로세스 포인터 얻기

	//프로세스 스택 제어
	virtual void NewProcess(int proc_id, int ex1 = 0, int ex2 = 0, int ex3 = 0, int ex4 = 0);
	virtual void NewProcess(int proc_id, char* str1, char* str2 = NULL, char* str3 = NULL, char* str4 = NULL);
	virtual void StopCurrentProcess();
	virtual void ClearAllProcess();

	//프로세스 실행
	virtual bool Process();

	CProcessManager(void);
	virtual ~CProcessManager(void);
};