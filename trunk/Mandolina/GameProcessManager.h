//말빨동굴 프로세스 관리자
//2010. 7. 31
//박동흥

#pragma once
#include "processmanager.h"

//프로세스ID
enum ProcessID
{
	proc_logo,
	proc_mainmenu,
	proc_story,
	proc_rpg,
	proc_srpg,
	proc_note
};

//게임 데이터
struct PlayData
{
	int hp;
	int stage;
	int final_answer;
};

class CGameProcessManager : public CProcessManager
{
private:
	static CGameProcessManager* p_instance;	//싱글톤
	
	PlayData playdata;	//게임 플레이 데이터

public:
	//인스턴스 얻기
	static CGameProcessManager* GetInstance()
	{
		if(!p_instance)
		{
			p_instance = new CGameProcessManager();
		}

		return p_instance;
	}

	void InitPlayData();	//데이터 초기화
	bool Process();			//프로세스 처리

	CProcess* CreateProcess(ProcessStack* p_stack);	//프로세스 생성

	//생성자, 소멸자
	CGameProcessManager(void);
	~CGameProcessManager(void);
};
