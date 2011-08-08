//�������� ���μ��� ������
//2010. 7. 31
//�ڵ���

#pragma once
#include "processmanager.h"

//���μ���ID
enum ProcessID
{
	proc_logo,
	proc_mainmenu,
	proc_story,
	proc_rpg,
	proc_srpg,
	proc_note
};

//���� ������
struct PlayData
{
	int hp;
	int stage;
	int final_answer;
};

class CGameProcessManager : public CProcessManager
{
private:
	static CGameProcessManager* p_instance;	//�̱���
	
	PlayData playdata;	//���� �÷��� ������

public:
	//�ν��Ͻ� ���
	static CGameProcessManager* GetInstance()
	{
		if(!p_instance)
		{
			p_instance = new CGameProcessManager();
		}

		return p_instance;
	}

	void InitPlayData();	//������ �ʱ�ȭ
	bool Process();			//���μ��� ó��

	CProcess* CreateProcess(ProcessStack* p_stack);	//���μ��� ����

	//������, �Ҹ���
	CGameProcessManager(void);
	~CGameProcessManager(void);
};
