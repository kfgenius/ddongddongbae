//#include <vector>

//using namespace std;
#include <stdio.h>

#include "GameProcess.h"
#include "Unit.h"

//�� �Ѱ�
#define LAYER_MAX	3
#define MAP_XMAX	80
#define MAP_YMAX	60
#define TILESIZE	32
#define TILE_MAX	120

//���� ����
#define UNIT_MAX	128

//��ã��
#define MOVE_DISABLE	9999

//////////////////////////////
//Ÿ�� ����
struct TileData
{
	unsigned char ground;	//�ٴ�
	unsigned char object;	//���� ����
	unsigned char unit;		//�� �ִ� ����
	unsigned char evnt;		//�̺�Ʈ
	unsigned char shadow;	//�׸��� ����
	
	//�� ã���
	int move;				//�ʿ� �̵���
	int road;				//���� �˻��ϴ� ��
	int distance;			//�Ÿ��� ��Ÿ���� ��
};

struct MapPoint
{
	int x;
	int y;
};

//////////////////////////////
//���� Ŭ����
class CSRPG : public CGameProcess
{
private:
	//�� ����
	bool battle;	//��������
	char map_name[80];
	unsigned char x_size, y_size;
	TileData map[MAP_XMAX][MAP_YMAX];

	//Ÿ�� �Ӽ�
	int tile_mov[2][TILE_MAX];		//�ʿ��� �̵���
	int tile_attr[2][TILE_MAX];		//�Ӽ�
	int tile_damage[2][TILE_MAX];	//�����
	CUnitData unitdata[TILE_MAX];

	//��ũ�� ����
	int scroll_x, scroll_y;
	int next_scroll_x, next_scroll_y;

	//���� ����
	CUnit unit[UNIT_MAX];
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

	//���� ��Ʈ��(����/������)
	void NormalControl();
	void BattleControl();

public:
	//�� �ε�
	bool Load(char* map_name, bool battle);

	//���� �帧 ����
	void Control();		//Ű �Է�
	void Process();		//���� ó��
	void Render();		//ȭ�� �׸���

	//���� ����
	void SetHeroXY(int x, int y);	//���ΰ� ��ǥ ����
	void SetAutoHeroXY(int ev);		//�ڵ� ��ǥ ����
	CUnit* GetUnit(int id);			//���� ���� ���
	void SetActiveUnit(int id);		//Ȱ��ȭ ���� ����

	//����, �ҷ�����
	void SaveGame(FILE* save_fp);
	void LoadGame(FILE* load_fp);

	//�̺�Ʈ ����
	void InitEvent();
	int GetEventNo();

	//��ã�� ����
	void SetStartPoint(int x, int y, int move_max);
	void SetEndPoint(int x, int y);
	void FindRoad(int x, int y, int expense, int move_max=99);
	bool MoveNextPointFast(int move);
	bool MoveNextPoint(int move);
	bool MoveNextPointStupid(int move, int iq);

	//������, �Ҹ���
	CSRPG();
	~CSRPG();
};
