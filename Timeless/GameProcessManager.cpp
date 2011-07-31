#include "GameProcessManager.h"

//프로세스들
#include "Logo.h"
#include "MainMenu.h"
#include "Story.h"
#include "RPG.h"
#include "SRPG.h"

//답 종류
#define FINAL_ANSWER_MAX	4

//던젼 종류
enum
{
	stage_start
};

//생성자
CGameProcessManager::CGameProcessManager()
{
	//그래픽 읽기
	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0, 0, 255));
	jdd->LoadPicture("SPRITE", "data\\sprite.gif", &jpi, TRUE);

	//데이터 초기화
	InitPlayData();

	//시작 프로세스를 넣자
	NewProcess(proc_rpg, "MAP\\MAP00", "User\\tile1.gif", "User\\tile2.gif");
	NewProcess(proc_srpg, "MAP\\MAP60", "User\\tile1.gif", "User\\tile2.gif");
	//NewProcess(proc_story);
	//NewProcess(proc_logo);
}

//소멸자
CGameProcessManager::~CGameProcessManager()
{
	jdd->DeleteSurface("SPRITE");
}

//플레이 데이터 초기화
void CGameProcessManager::InitPlayData()
{
	playdata.hp = 10;
	playdata.stage = stage_start;
	playdata.final_answer = rand() % FINAL_ANSWER_MAX;
}

//흐름 처리
bool CGameProcessManager::Process()
{
	//처리할 스택이 없으면 리턴
	if(IsEmptyStack())
	{
		return FALSE;
	}

	//프로세스 처리
	CProcessManager::Process();

	//현 프로세스 처리
	if(GetCurrentProcess())
	{
	    GetCurrentProcess()->Process();			//프로세스 처리
	}

	return TRUE;
}

//새로운 프로세스 생성
CProcess* CGameProcessManager::CreateProcess(ProcessStack* p_stack)
{
	//스택 데이터 건네 받기
	const int proc_id = p_stack->proc_id;
	const int ex1 = p_stack->ex1;
	const int ex2 = p_stack->ex2;
	const int ex3 = p_stack->ex3;
	const int ex4 = p_stack->ex4;

	char* str1 = p_stack->str1;
	char* str2 = p_stack->str2;
	char* str3 = p_stack->str3;
	char* str4 = p_stack->str4;

	//새 프로세스 생성
	switch(proc_id)
	{
		case proc_logo :	return (CProcess*)new CLogo;
							break;

		case proc_mainmenu : return (CProcess*)new CMainMenu;
							 break;

		case proc_story : return (CProcess*)new CStory;
							 break;

		case proc_rpg : return (CProcess*)new CRPG(str1, str2, str3);
						 break;

		case proc_srpg : return (CProcess*)new CSRPG(str1, str2, str3);
						 break;
	}
	
	return NULL;
}
