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
		if(delay > 100)
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

//이동 가능성
int CGameMap::GetMoveSpeed(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return 0;

	if(map[x][y].unit!=0xff)return 0;									//유닛이 있으면 이동 불가능
	else if(map[x][y].object!=0xff)return TILESIZE/(2<<map[x][y].move);	//2층 타일에 의한 이동
	else if(tile_attr[0][map[x][y].ground]>0)return 0;					//평상시엔 일반 길 외에는 이동 불가능
	else return TILESIZE/(2<<map[x][y].move);							//일반 1층 타일 이동
}