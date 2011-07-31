#include "GameProcessManager.h"

//���μ�����
#include "Logo.h"
#include "MainMenu.h"
#include "Story.h"
#include "RPG.h"
#include "SRPG.h"

//�� ����
#define FINAL_ANSWER_MAX	4

//���� ����
enum
{
	stage_start
};

//������
CGameProcessManager::CGameProcessManager()
{
	//�׷��� �б�
	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0, 0, 255));
	jdd->LoadPicture("SPRITE", "data\\sprite.gif", &jpi, TRUE);

	//������ �ʱ�ȭ
	InitPlayData();

	//���� ���μ����� ����
	NewProcess(proc_rpg, "MAP\\MAP00", "User\\tile1.gif", "User\\tile2.gif");
	NewProcess(proc_srpg, "MAP\\MAP60", "User\\tile1.gif", "User\\tile2.gif");
	//NewProcess(proc_story);
	//NewProcess(proc_logo);
}

//�Ҹ���
CGameProcessManager::~CGameProcessManager()
{
	jdd->DeleteSurface("SPRITE");
}

//�÷��� ������ �ʱ�ȭ
void CGameProcessManager::InitPlayData()
{
	playdata.hp = 10;
	playdata.stage = stage_start;
	playdata.final_answer = rand() % FINAL_ANSWER_MAX;
}

//�帧 ó��
bool CGameProcessManager::Process()
{
	//ó���� ������ ������ ����
	if(IsEmptyStack())
	{
		return FALSE;
	}

	//���μ��� ó��
	CProcessManager::Process();

	//�� ���μ��� ó��
	if(GetCurrentProcess())
	{
	    GetCurrentProcess()->Process();			//���μ��� ó��
	}

	return TRUE;
}

//���ο� ���μ��� ����
CProcess* CGameProcessManager::CreateProcess(ProcessStack* p_stack)
{
	//���� ������ �ǳ� �ޱ�
	const int proc_id = p_stack->proc_id;
	const int ex1 = p_stack->ex1;
	const int ex2 = p_stack->ex2;
	const int ex3 = p_stack->ex3;
	const int ex4 = p_stack->ex4;

	char* str1 = p_stack->str1;
	char* str2 = p_stack->str2;
	char* str3 = p_stack->str3;
	char* str4 = p_stack->str4;

	//�� ���μ��� ����
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
