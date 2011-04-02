#include "unit.h"
#include "hash.h"

#include <stdio.h>
//#include "donglib.h"

//맵 관련
/*#define UNIT_Y		40
#define UNIT_PY		8	//유닛은 세로가 40이므로 8픽셀 위에 찍음

//그림 이름
#define PIC_MAP		"Map"
#define PIC_TILE1	"tile1"
#define PIC_TILE2	"tile2"

//이벤트 종류
#define EVENT_NONE		-1
#define EVENT_GO_OUT	999

//애니 번호
#define HERO			0
#define ANI_TILE		128

//애니메이션
CAnimation ani;*/
#define TILESIZE	32

//이동 보너스
#define NORMAL_MOVE		0	//보통
#define WATER_MOVE		1	//물
#define FIRE_MOVE		2	//불
#define SPECIAL_MOVE	3	//유격대
#define SKY_MOVE		4	//비행
#define GHOST_MOVE		5	//유령

////////////////////////////////////////////////////////////
// CUnit 메소드

//유닛 능력치 데이터
int unit_data[][20] = {
//   HP		힘	민첩이동력	이동보너스	
	{30,	0,	12,	12,		NORMAL_MOVE},	//휴가루
	{99,	0,	8,	12,		GHOST_MOVE},	//루시버
	{30,	0,	9,	12,		FIRE_MOVE},		//가미얼
	{20,	0,	10,	12,		WATER_MOVE},	//시크벨
	{20,	0,	15,	12,		SKY_MOVE},		//미버가
	{30,	5,	5,	10,		NORMAL_MOVE},	//땅마왕
	{99,	3,	12,	12,		NORMAL_MOVE},	//휴루시
	{10,	1,	10,	12,		NORMAL_MOVE},	//남자
	{10,	1,	10,	12,		NORMAL_MOVE},	//여자
	{10,	0,	8,	12,		NORMAL_MOVE},	//위생병
	{15,	3,	10,	12,		SPECIAL_MOVE},	//유격대
	{12,	2,	15,	14,		SPECIAL_MOVE}	//도둑
};

CUnit::CUnit()
{
	life=false;
	moving=false;
	SetPos(0,0);
}

//설정
void CUnit::Set(int id)
{
	life = true;
	this->id = id;

	if(id<6)team = 0;
	else team = 1;

	char unit_name[20];
	sprintf(unit_name, "Unit%d", id);
	pic_id = CHash::GetInstance()->GetHash(unit_name);

	//능력치 초기값 지정
	hp_max = unit_data[id][0];
	power = unit_data[id][1];
	quickness = unit_data[id][2];
	move = unit_data[id][3];
	move_bonus = unit_data[id][4];

	//공통 초기값
	ap = 100;
	hp = hp_max;
}

//사망
void CUnit::Die()
{
	life = false;
}

//생존 상태 얻기
bool CUnit::GetLife()
{
	return life;
}

//ID 얻기
int CUnit::GetID()
{
	return id;
}

int CUnit::GetPictureID()
{
	return pic_id;
}

//좌표 얻기
int CUnit::GetX()
{
	return x;
}

int CUnit::GetY()
{
	return y;
}

int CUnit::GetPX()
{
	return px;
}

int CUnit::GetPY()
{
	return py;
}

int CUnit::GetRealX()
{
	return x * TILESIZE + px;
}

int CUnit::GetRealY()
{
	return y * TILESIZE + py;
}

//방향 얻기, 설정
int CUnit::GetDir()
{
	return dir;
}

void CUnit::SetDir(int dir)
{
	this->dir = dir;
}

//바라보고 있는 곳 구하기
int CUnit::GetDirX()
{
	if(dir==DIR_LEFT)return x-1;
		else if(dir==DIR_RIGHT)return x+1;
		else return x;
}

int CUnit::GetDirY()
{
	if(dir==DIR_UP)return y-1;
		else if(dir==DIR_DOWN)return y+1;
		else return y;
}

//이벤트 번호 지정/구하기
void CUnit::SetEventNo(int event_no)
{
	this->event_no = event_no;
}

int CUnit::GetEventNo()
{
	return event_no;
}

//위치 지정
void CUnit::SetPos(int x, int y)
{
	this->x = x;
	this->y = y;
	px = py = 0;
	moving = false;
}

//이동 속도 지정
void CUnit::SetMoveSpeed(int move_speed)
{
	this->move_speed = move_speed;
}

//이동
bool CUnit::Move(int dir)
{
	if(moving)return false;
	this->dir = dir;

	if(dir == DIR_LEFT)
	{
		--x;
		px=TILESIZE;
	}
	else if(dir == DIR_UP)
	{
		--y;
		py=TILESIZE;
	}
	else if(dir == DIR_RIGHT)
	{
		++x;
		px=-TILESIZE;
	}
	else if(dir == DIR_DOWN)
	{
		++y;
		py=-TILESIZE;
	}

	moving = true;
	return true;
}

void CUnit::Action()
{
	if(!moving)return;

	//좌로 이동
	if(px>0)
	{
		px -= move_speed;
	}
	//우로 이동
	else if(px<0)
	{
		px += move_speed;
	}
	//위로 이동
	else if(py>0)
	{
		py -= move_speed;
	}
	//아래로 이동
	else if(py<0)
	{
		py += move_speed;
	}
	//이동이 끝나면 키 입력 가능
	else moving = false;
}