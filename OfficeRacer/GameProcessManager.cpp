#include "GameProcessManager.h"

//���μ�����
#include "Dungeon.h"

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
	jdd->LoadPicture("SPRITE", "data\\sprite.gif", &jpi, true);

	//������ �ʱ�ȭ
	InitPlayData();

	//���� ���μ����� ����
	NewProcess(proc_dungeon);
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
		return false;
	}

	//���μ��� ó��
	CProcessManager::Process();

	//�� ���μ��� ó��
	if(GetCurrentProcess())
	{
		GetCurrentProcess()->Control();	//�Է� ó��
	    GetCurrentProcess()->Process();	//���μ��� ó��
		GetCurrentProcess()->Render();	//��� ó��
	}

	return true;
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

	//�� ���μ��� ����
	switch(proc_id)
	{
		case proc_dungeon :	return (CProcess*)new CDungeon;
							break;
	}
	
	return NULL;
}
