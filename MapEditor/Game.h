#include <vector>

using namespace std;

//맵 한계
#define LAYER_MAX	3
#define MAP_XMAX	80
#define MAP_YMAX	60
#define TILESIZE	32

//유닛 정보
#define UNIT_MAX	128
#define HERO		0

//유닛 방향
#define DIR_LEFT	0
#define DIR_UP		1
#define DIR_RIGHT	2
#define DIR_DOWN	3

//이벤트
struct EventStack
{
	int event_id;
	int value[4];
};

//위치
struct MapPos
{
	int map_id;
	int x, y;
};

//////////////////////////////
//유닛 클래스
class CUnit
{
private:
	bool life;
	int id;
	int x, y;
	int px, py;			//실좌표를 표현하기 위한 변수(x*TILESIZE+px 식으로 사용)

	//이동 관련
	bool moving;		//이동 중인지 확인(이동중에는 이동 불가능)
	int move_speed;		//이동용 스피드(일시적)
	int dir;

	//능력치
	int lv, exp, next_exp;
	int hp, hp_max;
	int mp, mp_max;
	int power, defense, wisdom, quickness, move;

public:
	//생성자
	CUnit()
	{
		life=false;
		moving=false;
		SetPos(0,0);
	}

	//설정
	void Set(int id)
	{
		life = true;
		this->id = id;
	}

	//ID 얻기
	int GetID()
	{
		return id;
	}

	//좌표 얻기
	int GetX()
	{
		return x;
	}

	int GetY()
	{
		return y;
	}

	//실좌표 얻기
	int GetRealX()
	{
		return x*TILESIZE+px;
	}

	int GetRealY()
	{
		return y*TILESIZE+py;
	}

	//방향 얻기, 설정
	int GetDir()
	{
		return dir;
	}

	void SetDir(int dir)
	{
		this->dir = dir;
	}

	//바라보고 있는 곳 구하기
	int GetDirX()
	{
		if(dir==DIR_LEFT)return x-1;
			else if(dir==DIR_RIGHT)return x+1;
			else x;
	}

	int GetDirY()
	{
		if(dir==DIR_UP)return y-1;
			else if(dir==DIR_DOWN)return y+1;
			else y;
	}

	//위치 지정
	void SetPos(int x, int y)
	{
		this->x = x;
		this->y = y;
		px = py = 0;
		moving = false;
	}

	//이동 속도 지정
	void SetMoveSpeed(int move_speed)
	{
		this->move_speed = move_speed;
	}

	//이동
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

		//좌로 이동
		if(px>0)
		{
			//px=Max(px - move_speed, 0);
			px -= move_speed;
		}
		//우로 이동
		else if(px<0)
		{
			//px=Min(px + move_speed, 0);
			px += move_speed;
		}
		//위로 이동
		else if(py>0)
		{
			//py=Max(py - move_speed, 0);
			py -= move_speed;
		}
		//아래로 이동
		else if(py<0)
		{
			//py=Min(py + move_speed, 0);
			py += move_speed;
		}
		//이동이 끝나면 키 입력 가능
		else moving = false;
	}
};

//////////////////////////////
//게임 클래스
class CGame  
{
private:
	//맵 정보
	unsigned char x_size, y_size;
	unsigned char map[LAYER_MAX][MAP_XMAX][MAP_YMAX];
	unsigned char eventmap[MAP_XMAX][MAP_YMAX];
	int map_id;

	//맵 위치 기억
	vector<MapPos> map_pos;

	//스크롤 변수
	int scroll_x, scroll_y;		
	int next_scroll_x, next_scroll_y;

	//유닛 정보
	CUnit unit[UNIT_MAX];
	int unit_order[UNIT_MAX];
	int unit_max;

	//이벤트
	EventStack event_stack;

	int tile1_mov[120];
	int tile2_mov[120];

	/////////////////////////////////////////////
	//메소드
	bool Load(int map_id);		//맵 로드
	void CreateUnit();			//유닛그림 생성(나중에 삭제)
	void DrawGround();			//맵 미리 그리기

	int	GetMoveSpeed(int x, int y);	//이동 가능성 검사
	int Crush(int x, int y);		//이동시 이벤트 검사
	int Talk(int x, int y);			//대화시 이벤트 검사

	void InsertEvent(int event_id, int v1=-1, int v2=-1, int v3=-1, int v4=-1);	//이벤트 넣기

	void UnitMove(int id, int dir);	//유닛 이동
	void UnitSort();				//유닛 Y좌표 소트

	void InitScroll();	//스크롤을 초기화

public:
	void Start();		//게임 시작
	void Process();		//게임 처리
	void Draw();		//화면 그리기 
	void End();			//뒷 정리

	CGame();
	virtual ~CGame();
};
