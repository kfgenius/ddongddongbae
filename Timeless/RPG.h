#pragma once

#include <stdio.h>

#include "Unit.h"
#include "GameProcess.h"

//�� �Ѱ�
#define LAYER_MAX			3
#define MAP_WIDTH_MAX		80
#define MAP_HEIGHT_MAX		60
#define TILE_SIZE			32
#define TILE_SOURCE_SIZE	48
#define TILE_TYPE_MAX		18

//���� �Ѱ�
#define UNIT_TYPE_MAX		128

//���ΰ� ID
#define HERO	0

//�̵� �Ұ�
#define MOVE_DISABLE	9999

//��
enum
{
	party_ally,
	party_enemy,
	party_neutral
};

//////////////////////////////
//Ÿ�� ����
struct TileData
{
	unsigned char ground;	//�ٴ�
	unsigned char object;	//���� ����
	unsigned char unit;		//�� �ִ� ����
	unsigned char event_no;	//�̺�Ʈ

	//�� ã���
	int move;		//�Ҹ�Ǵ� �̵���
	int road;		//���� �˻��ϴ� ��
	int distance;	//�Ÿ��� ��Ÿ���� ��
};

struct MapPoint
{
	int x;
	int y;
};

//////////////////////////////
//���� Ŭ����
class CRPG : public CGameProcess
{
private:
	//Ÿ�� ���� �Ѱ�
	int tile_type_max;

	//�̺�Ʈ �˻�
	int Crush(int x, int y);		//�̵��� �̺�Ʈ �˻�
	int Talk(int x, int y);			//��ȭ�� �̺�Ʈ �˻�

	//����
	void Control();

public:
	//�� ó�� ���̵���
	BOOL first_fade_in;

	//��ũ�� ����
	int scroll_x, scroll_y;
	int next_scroll_x, next_scroll_y;

	//�̺�Ʈ
	int event_no;

	//��ã�� ����
	MapPoint point;		//���� �繰�� �ִ� ��ġ

	//���� ����
	int mode;		//���
	int result;		//�����

	//�� ����
	char map_name[80];
	unsigned char x_size, y_size;
	TileData map[MAP_WIDTH_MAX][MAP_HEIGHT_MAX];

	//���� ����
	CUnit unit[UNIT_TYPE_MAX];
	int unit_max;
	int active_unit;	//Ȱ��ȭ�� ����

	//Ÿ�� �Ӽ�
	int tile_mov[2][TILE_TYPE_MAX];		//�ʿ��� �̵���
	int tile_attr[2][TILE_TYPE_MAX];		//�Ӽ�
	int tile_damage[2][TILE_TYPE_MAX];	//�����
	CUnitData unitdata[UNIT_TYPE_MAX];

	//�ִϸ��̼�
	CAnimation ani;

	//���� ����
	virtual void CreateUnit();			//���ֱ׸� ����(���߿� ����)
	virtual void DrawGround();			//�� �̸� �׸���
	virtual void UnitMove(int id, int dir);	//���� �̵�
	virtual int FindUnit(int x, int y);	//�ش���ġ�� ���� ã��

	//�̵� ����
	virtual int	GetMoveSpeed(int x, int y);	//�̵� ���ɼ� �˻�

	//��ũ�� ����
	virtual void InitScroll(int unit_id);	//��ũ���� ������ �߽����� �ʱ�ȭ
	virtual void Focus(int unit_id);		//��ũ���� �������� �ڿ������� �̵�

	//�� ���
	virtual void Draw1F();
	virtual void Draw2F();
	virtual void Draw3F();

	//���� �帧 ����
	virtual void Process();

	//���� ����
	virtual void SetHeroXY(int x, int y);	//���ΰ� ��ǥ ����
	virtual void SetAutoHeroXY(int ev);		//�ڵ� ��ǥ ����
	virtual CUnit* GetUnit(int id);			//���� ���� ���
	virtual void SetActiveUnit(int id);		//Ȱ��ȭ ���� ����

	//�̺�Ʈ ����
	virtual void InitEvent();
	virtual int GetEventNo();

	//������, �Ҹ���
	CRPG(char* map_name, char* tile1_name, char* tile2_name);
	virtual ~CRPG();
};