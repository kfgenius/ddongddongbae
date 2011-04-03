#include "rpg.h"

////////////////////////////////////////////////////////////
// CRPG 메소드

CRPG::CRPG()
{
}

CRPG::~CRPG()
{
}

//조작
void CRPG::Control()
{
	CGameMap::Control();

	//스크립트 처리 중일 때는 조작 불가
	if(m_script->IsReady())return;

	//이동
	if(GetKey(vkey_up,0))
	{
		UnitMove(HERO, DIR_UP);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}
	else if(GetKey(vkey_down,0))
	{
		UnitMove(HERO, DIR_DOWN);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}
	else if(GetKey(vkey_left,0))
	{
		UnitMove(HERO, DIR_LEFT);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}
	else if(GetKey(vkey_right,0))
	{
		UnitMove(HERO, DIR_RIGHT);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}
}

void CRPG::Process()
{
	CGameMap::Process();

	//NPC 행동
	for(int i = HERO + 1; i < unit_max; ++i)
	{
		static int delay;
		++delay;
		if(delay>100)
		{
			int dir = rand()%4;
			UnitMove(i, dir);
			ani.GetAni(i)->SetDirection((WalkDirection)dir);
			delay=0;
		}
	}

	//포커스
	InitScroll(HERO);
}