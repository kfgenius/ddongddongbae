#include <vector>

using namespace std;

//�� �Ѱ�
#define LAYER_MAX	3
#define MAP_XMAX	80
#define MAP_YMAX	60
#define TILESIZE	32
#define TILE_MAX	120

//���� ����
#define UNIT_MAX	128

//���� ����
#define DIR_UP		0
#define DIR_LEFT	1
#define DIR_DOWN	2
#define DIR_RIGHT	3

/////////////////////////////
//�� �����Ϳ��� �Ѿ���� ��
struct CUnitData
{
	int hp;
	int mp;

	int pow;
	int mpow;
	int def;
	int mdef;
	int agl;
	int mov;
	int jump;
	int swim;
	int see;
	int act;

	int attr;
};

//////////////////////////////
//���� Ŭ����
class CUnit
{
private:
	bool life;
	int id;
	int x, y;
	int px, py;			//����ǥ�� ǥ���ϱ� ���� ����(x*TILESIZE+px ������ ���)

	//�̵� ����
	bool moving;		//�̵� ������ Ȯ��(�̵��߿��� �̵� �Ұ���)
	int move_speed;		//�̵��� ���ǵ�(�Ͻ���)
	int move_delay;		//�̵� �����ð�(�̵� �ӵ� ����)
	int dir;			//���� ����

	//�ɷ�ġ
	int lv, exp, next_exp;
	int hp, hp_max;
	int mp, mp_max;
	int power, defense, wisdom, quickness, move;
	int pattern;

public:
	//������
	CUnit()
	{
		life=false;
		moving=false;
		move_delay=0;
		SetPos(0,0);
	}

	//����
	void Set(int id)
	{
		life = true;
		this->id = id;
	}

	//���
	void Die()
	{
		life = false;
	}

	//���� ���� ���
	bool GetLife()
	{
		return life;
	}

	//ID ���
	int GetID()
	{
		return id;
	}

	//��ǥ ���
	int GetX()
	{
		return x;
	}

	int GetY()
	{
		return y;
	}

	//����ǥ ���
	int GetRealX()
	{
		return x*TILESIZE+px;
	}

	int GetRealY()
	{
		return y*TILESIZE+py;
	}

	//���� ���, ����
	int GetDir()
	{
		return dir;
	}

	void SetDir(int dir)
	{
		this->dir = dir;
	}

	//�ٶ󺸰� �ִ� �� ���ϱ�
	int GetDirX()
	{
		if(dir==DIR_LEFT)return x-1;
			else if(dir==DIR_RIGHT)return x+1;
			else return x;
	}

	int GetDirY()
	{
		if(dir==DIR_UP)return y-1;
			else if(dir==DIR_DOWN)return y+1;
			else return y;
	}

	//��ġ ����
	void SetPos(int x, int y)
	{
		this->x = x;
		this->y = y;
		px = py = 0;
		moving = false;
	}

	//�̵� �ӵ� ����
	void SetMoveSpeed(int move_speed)
	{
		this->move_speed = move_speed;
	}

	//�̵�
	void Move(int dir)
	{
		if(moving)return;
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
	}

	void Action()
	{
		if(!moving)return;

		//�·� �̵�
		if(px>0)
		{
			//px=Max(px - move_speed, 0);
			px -= move_speed;			
		}
		//��� �̵�
		else if(px<0)
		{
			//px=Min(px + move_speed, 0);
			px += move_speed;			
		}
		//���� �̵�
		else if(py>0)
		{
			//py=Max(py - move_speed, 0);
			py -= move_speed;			
		}
		//�Ʒ��� �̵�
		else if(py<0)
		{
			//py=Min(py + move_speed, 0);
			py += move_speed;
		}
		//�̵��� ������ Ű �Է� ����
		else moving = false;
	}
};

//////////////////////////////
//���� Ŭ����
class CGame  
{
private:
	//�� ����
	char map_name[80];
	unsigned char x_size, y_size;
	unsigned char map[LAYER_MAX][MAP_XMAX][MAP_YMAX];
	unsigned char eventmap[MAP_XMAX][MAP_YMAX];

	//Ÿ�� �Ӽ�
	int tile_mov[2][TILE_MAX];
	int tile_attr[2][TILE_MAX];
	int tile_damage[2][TILE_MAX];
	CUnitData unitdata[TILE_MAX];

	//��ũ�� ����
	int scroll_x, scroll_y;		
	int next_scroll_x, next_scroll_y;

	//���� ����
	CUnit unit[UNIT_MAX];
	int unit_order[UNIT_MAX];
	int unit_max;

	//�̺�Ʈ
	int event_no;

	/////////////////////////////////////////////
	//�޼ҵ�
	void CreateUnit();			//���ֱ׸� ����(���߿� ����)
	void DrawGround();			//�� �̸� �׸���

	int	GetMoveSpeed(int x, int y);	//�̵� ���ɼ� �˻�
	int Crush(int x, int y);		//�̵��� �̺�Ʈ �˻�
	int Talk(int x, int y);			//��ȭ�� �̺�Ʈ �˻�

	void InsertEvent(int event_id, int v1=-1, int v2=-1, int v3=-1, int v4=-1);	//�̺�Ʈ �ֱ�

	void UnitMove(int id, int dir);	//���� �̵�
	void UnitSort();				//���� Y��ǥ ��Ʈ
	int FindUnit(int x, int y);	//�ش���ġ�� ���� ã��

	void InitScroll();	//��ũ���� �ʱ�ȭ

public:
	void Start();				//���� ����
	bool Load(char* map_name);	//�� �ε�
	void Control();				//Ű �Է�
	int Process();				//���� ó��
	void Draw();				//ȭ�� �׸��� 
	void End();					//�� ����

	void SetHeroXY(int x, int y);	//���ΰ� ��ǥ ����
	void SetAutoHeroXY();			//�ڵ� ��ǥ ����
	CUnit* GetUnit(int id);			//���� ���� ���

	void SaveGame(FILE* save_fp);	//���� ���� ����
	void LoadGame(FILE* load_fp);	//���� ���� �ҷ�����

	CGame();
	~CGame();
};
