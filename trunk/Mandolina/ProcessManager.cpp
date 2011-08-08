#include <windows.h>
#include <stdio.h>

#include "ProcessManager.h"

//생성자
CProcessManager::CProcessManager(void)
{
	stack_count = 0;
}

//소멸자
CProcessManager::~CProcessManager(void)
{
}

//스택이 비었는지 확인
bool CProcessManager::IsEmptyStack()
{
	if(stack_count == 0)
	{
		return TRUE;
	}
	
	return FALSE;
}

//프로세스 얻기
CProcess* CProcessManager::GetCurrentProcess()
{
	if(stack_count == 0)
	{
		return NULL;
	}

	return stack[stack_count - 1].p_process;
}

//프로세스 생성
CProcess* CProcessManager::CreateProcess(ProcessStack* p_stack)
{
	//상속받은 후 오버로딩해서 사용
	//(원래 순수 가상함수였는데 싱글톤을 위해서 이렇게 선언)

	return NULL;
}

//스택에 프로세스 추가
void CProcessManager::NewProcess(int proc_id, int ex1, int ex2, int ex3, int ex4)
{
	//스택이 가득 참
	if(stack_count >= STACK_MAX - 1)
	{
		printf("Warning : Stack is full");
		return;	
	}

	//스택에 넣기
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
	//스택이 가득 참
	if(stack_count >= STACK_MAX - 1)
	{
		printf("Warning : Stack is full");
		return;	
	}

	//스택에 넣기
	stack[stack_count].proc_id = proc_id;
	stack[stack_count].str1 = str1;
	stack[stack_count].str2 = str2;
	stack[stack_count].str3 = str3;
	stack[stack_count].str4 = str4;
	stack[stack_count].p_process = NULL;

	++stack_count;
}


//스택 제거
void CProcessManager::StopCurrentProcess()
{
	if(stack_count > 0)
	{
		//현재 프로세스 삭제
		if(stack[stack_count - 1].p_process)
		{
			delete stack[stack_count - 1].p_process;
			stack[stack_count - 1].p_process = NULL;
		}

		--stack_count;
	}
}

//스택 비우기
void CProcessManager::ClearAllProcess()
{
	for(int i = stack_count - 1; i >= 0; --i)
	{
		//프로세스들 삭제
		if(stack[i].p_process)
		{
			delete stack[i].p_process;
			stack[i].p_process = NULL;
		}
	}

	stack_count = 0;
}

//프로세스 처리
bool CProcessManager::Process()
{
	//새 프로세스 시작
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