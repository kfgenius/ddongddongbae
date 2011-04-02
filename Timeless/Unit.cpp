#include "unit.h"
#include "hash.h"

#include <stdio.h>
//#include "donglib.h"

//�� ����
/*#define UNIT_Y		40
#define UNIT_PY		8	//������ ���ΰ� 40�̹Ƿ� 8�ȼ� ���� ����

//�׸� �̸�
#define PIC_MAP		"Map"
#define PIC_TILE1	"tile1"
#define PIC_TILE2	"tile2"

//�̺�Ʈ ����
#define EVENT_NONE		-1
#define EVENT_GO_OUT	999

//�ִ� ��ȣ
#define HERO			0
#define ANI_TILE		128

//�ִϸ��̼�
CAnimation ani;*/
#define TILESIZE	32

//�̵� ���ʽ�
#define NORMAL_MOVE		0	//����
#define WATER_MOVE		1	//��
#define FIRE_MOVE		2	//��
#define SPECIAL_MOVE	3	//���ݴ�
#define SKY_MOVE		4	//����
#define GHOST_MOVE		5	//����

////////////////////////////////////////////////////////////
// CUnit �޼ҵ�

//���� �ɷ�ġ ������
int unit_data[][20] = {
//   HP		��	��ø�̵���	�̵����ʽ�	
	{30,	0,	12,	12,		NORMAL_MOVE},	//�ް���
	{99,	0,	8,	12,		GHOST_MOVE},	//��ù�
	{30,	0,	9,	12,		FIRE_MOVE},		//���̾�
	{20,	0,	10,	12,		WATER_MOVE},	//��ũ��
	{20,	0,	15,	12,		SKY_MOVE},		//�̹���
	{30,	5,	5,	10,		NORMAL_MOVE},	//������
	{99,	3,	12,	12,		NORMAL_MOVE},	//�޷��
	{10,	1,	10,	12,		NORMAL_MOVE},	//����
	{10,	1,	10,	12,		NORMAL_MOVE},	//����
	{10,	0,	8,	12,		NORMAL_MOVE},	//������
	{15,	3,	10,	12,		SPECIAL_MOVE},	//���ݴ�
	{12,	2,	15,	14,		SPECIAL_MOVE}	//����
};

CUnit::CUnit()
{
	life=false;
	moving=false;
	SetPos(0,0);
}

//����
void CUnit::Set(int id)
{
	life = true;
	this->id = id;

	if(id<6)team = 0;
	else team = 1;

	char unit_name[20];
	sprintf(unit_name, "Unit%d", id);
	pic_id = CHash::GetInstance()->GetHash(unit_name);

	//�ɷ�ġ �ʱⰪ ����
	hp_max = unit_data[id][0];
	power = unit_data[id][1];
	quickness = unit_data[id][2];
	move = unit_data[id][3];
	move_bonus = unit_data[id][4];

	//���� �ʱⰪ
	ap = 100;
	hp = hp_max;
}

//���
void CUnit::Die()
{
	life = false;
}

//���� ���� ���
bool CUnit::GetLife()
{
	return life;
}

//ID ���
int CUnit::GetID()
{
	return id;
}

int CUnit::GetPictureID()
{
	return pic_id;
}

//��ǥ ���
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

//���� ���, ����
int CUnit::GetDir()
{
	return dir;
}

void CUnit::SetDir(int dir)
{
	this->dir = dir;
}

//�ٶ󺸰� �ִ� �� ���ϱ�
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

//�̺�Ʈ ��ȣ ����/���ϱ�
void CUnit::SetEventNo(int event_no)
{
	this->event_no = event_no;
}

int CUnit::GetEventNo()
{
	return event_no;
}

//��ġ ����
void CUnit::SetPos(int x, int y)
{
	this->x = x;
	this->y = y;
	px = py = 0;
	moving = false;
}

//�̵� �ӵ� ����
void CUnit::SetMoveSpeed(int move_speed)
{
	this->move_speed = move_speed;
}

//�̵�
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

	//�·� �̵�
	if(px>0)
	{
		px -= move_speed;
	}
	//��� �̵�
	else if(px<0)
	{
		px += move_speed;
	}
	//���� �̵�
	else if(py>0)
	{
		py -= move_speed;
	}
	//�Ʒ��� �̵�
	else if(py<0)
	{
		py += move_speed;
	}
	//�̵��� ������ Ű �Է� ����
	else moving = false;
}