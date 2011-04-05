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

	int move;	//�Ҹ�Ǵ� �̵���
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

	//��ũ�� ����
	int scroll_x, scroll_y;
	int next_scroll_x, next_scroll_y;

	//���� ����
	CUnit unit[UNIT_TYPE_MAX];
	int unit_max;
	int active_unit;	//Ȱ��ȭ�� ����

	//�̺�Ʈ
	int event_no;

	//��ã�� ����
	MapPoint point;		//���� �繰�� �ִ� ��ġ

	//���� ����
	int mode;		//���
	int result;		//�����

	/////////////////////////////////////////////
	//�޼ҵ�

	//���� ����
	void CreateUnit();			//���ֱ׸� ����(���߿� ����)
	void DrawGround();			//�� �̸� �׸���
	void UnitMove(int id, int dir);	//���� �̵�
	int FindUnit(int x, int y);	//�ش���ġ�� ���� ã��

	//�̵� ����
	int	GetMoveSpeed(int x, int y);	//�̵� ���ɼ� �˻�
	int Crush(int x, int y);		//�̵��� �̺�Ʈ �˻�
	int Talk(int x, int y);			//��ȭ�� �̺�Ʈ �˻�

	//��ũ�� ����
	void InitScroll(int unit_id);	//��ũ���� ������ �߽����� �ʱ�ȭ
	void Focus(int unit_id);		//��ũ���� �������� �ڿ������� �̵�

	//����
	void Control();

public:
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

	//�� �ε�
	bool Load(char* map_name);

	//�� ���
	void Draw1F();
	void Draw2F();
	void Draw3F();

	//���� �帧 ����
	void Process();

	//���� ����
	void SetHeroXY(int x, int y);	//���ΰ� ��ǥ ����
	void SetAutoHeroXY(int ev);		//�ڵ� ��ǥ ����
	CUnit* GetUnit(int id);			//���� ���� ���
	void SetActiveUnit(int id);		//Ȱ��ȭ ���� ����

	//�̺�Ʈ ����
	void InitEvent();
	int GetEventNo();

	//������, �Ҹ���
	CRPG(char* map_name, char* tile1_name, char* tile2_name);
	~CRPG();
};