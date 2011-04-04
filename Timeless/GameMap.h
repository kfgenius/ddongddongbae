#include <stdio.h>

#include "Unit.h"
#include "Animation.h"

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

//////////////////////////////
//Ÿ�� ����
struct TileData
{
	unsigned char ground;	//�ٴ�
	unsigned char object;	//���� ����
	unsigned char unit;		//�� �ִ� ����
	unsigned char event_no;	//�̺�Ʈ
};

struct MapPoint
{
	int x;
	int y;
};

//////////////////////////////
//���� Ŭ����
class CGameMap
{
private:
	//�� ����
	char map_name[80];
	unsigned char x_size, y_size;
	TileData map[MAP_WIDTH_MAX][MAP_HEIGHT_MAX];

	//Ÿ�� �Ӽ�
	int tile_mov[2][TILE_TYPE_MAX];		//�ʿ��� �̵���
	int tile_attr[2][TILE_TYPE_MAX];		//�Ӽ�
	int tile_damage[2][TILE_TYPE_MAX];	//�����
	CUnitData unitdata[UNIT_TYPE_MAX];

	//�ִϸ��̼�
	CAnimation ani;

	//��ũ�� ����
	int scroll_x, scroll_y;
	int next_scroll_x, next_scroll_y;

	//���� ����
	CUnit unit[UNIT_TYPE_MAX];
	int unit_max;

	//���� ����
	void CreateUnit();			//���ֱ׸� ����(���߿� ����)
	void DrawGround();			//�� �̸� �׸���
	int FindUnit(int x, int y);		//�ش���ġ�� ���� ã��

	//��ũ�� ����
	void InitScroll(int unit_id);	//��ũ���� ������ �߽����� �ʱ�ȭ
	void Focus(int unit_id);		//��ũ���� �������� �ڿ������� �̵�

public:
	//�� ���
	void Draw1F();
	void Draw2F();
	void DrawNormalUnit();
	void DrawFlyUnit();

	//���� ����
	void SetHeroXY(int x, int y);	//���ΰ� ��ǥ ����
	void SetAutoHeroXY(int ev);		//�ڵ� ��ǥ ����
	CUnit* GetUnit(int id);			//���� ���� ���

	//������, �Ҹ���
	CGameMap(char* map_name, char* tile1_name, char* tile2_name);
	virtual ~CGameMap();
};
