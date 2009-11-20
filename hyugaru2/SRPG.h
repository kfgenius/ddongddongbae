#include <vector>

using namespace std;

//맵 한계
#define LAYER_MAX	3
#define MAP_XMAX	80
#define MAP_YMAX	60
#define TILESIZE	32
#define TILE_MAX	120

//유닛 정보
#define UNIT_MAX	128

//유닛 방향
#define DIR_UP		0
#define DIR_LEFT	1
#define DIR_DOWN	2
#define DIR_RIGHT	3

//길찾기
#define MOVE_DISABLE	9999

/////////////////////////////
//맵 에디터에서 넘어오는 값
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
//유닛 클래스
class CUnit
{
private:
	bool life;
	int id;
	int pic_id;
	int x, y;
	int px, py;		//실좌표를 표현하기 위한 변수(x*TILESIZE+px 식으로 사용)

	//이동 관련
	bool moving;	//이동 중인지 확인(이동중에는 이동 불가능)
	int move_speed;	//이동용 스피드(일시적)
	int dir;		//바라보는 방향

public:
	//능력치
	int lv, exp, next_exp;
	int hp, hp_max;
	int mp, mp_max;
	int power, defense, wisdom, quickness, move, ap;
	int pattern;
	int move_bonus;		//이동 보너스(물이나 불)

	//팀
	int team;

	//발생 이벤트
	int event_no;

	//유닛 배치, 삭제 관련
	void Set(int id);
	void Die();
	bool GetLife();

	//ID 얻기
	int GetID();
	int GetPictureID();

	//좌표 얻기
	int GetX();
	int GetY();
	int GetPX();
	int GetPY();
	int GetRealX();
	int GetRealY();

	//방향 얻기, 설정
	int GetDir();
	void SetDir(int dir);

	//바라보고 있는 곳 구하기
	int GetDirX();
	int GetDirY();

	//이벤트 번호 지정/구하기
	void SetEventNo(int event_no);
	int GetEventNo();

	//위치 지정
	void SetPos(int x, int y);

	//이동
	bool Move(int dir);
	void Action();
	void SetMoveSpeed(int move_speed);

	//생성자
	CUnit();
};

//////////////////////////////
//타일 정보
struct TileData
{
	unsigned char ground;	//바닥
	unsigned char object;	//위의 물건
	unsigned char unit;		//서 있는 유닛
	unsigned char evnt;		//이벤트
	unsigned char shadow;	//그림자 여부
	
	//길 찾기용
	int move;				//필요 이동력
	int road;				//길을 검색하는 맵
	int distance;			//거리를 나타내는 맵
};

struct MapPoint
{
	int x;
	int y;
};

//////////////////////////////
//게임 클래스
class CSRPG  
{
private:
	//맵 정보
	bool battle;	//전투여부
	char map_name[80];
	unsigned char x_size, y_size;
	TileData map[MAP_XMAX][MAP_YMAX];

	//타일 속성
	int tile_mov[2][TILE_MAX];		//필요한 이동력
	int tile_attr[2][TILE_MAX];		//속성
	int tile_damage[2][TILE_MAX];	//대미지
	CUnitData unitdata[TILE_MAX];

	//스크롤 변수
	int scroll_x, scroll_y;
	int next_scroll_x, next_scroll_y;

	//유닛 정보
	CUnit unit[UNIT_MAX];
	int unit_max;
	int active_unit;	//활성화된 유닛

	//이벤트
	int event_no;

	//길찾기 관련
	MapPoint point;		//지금 사물이 있는 위치

	//제어 관련
	int mode;		//모드
	int result;		//결과값

	/////////////////////////////////////////////
	//메소드

	//유닛 관련
	void CreateUnit();			//유닛그림 생성(나중에 삭제)
	void DrawGround();			//맵 미리 그리기
	void UnitMove(int id, int dir);	//유닛 이동
	int FindUnit(int x, int y);	//해당위치의 유닛 찾기

	//이동 관련
	int	GetMoveSpeed(int x, int y);	//이동 가능성 검사
	int Crush(int x, int y);		//이동시 이벤트 검사
	int Talk(int x, int y);			//대화시 이벤트 검사

	//스크롤 관련
	void InitScroll(int unit_id);	//스크롤을 유닛을 중심으로 초기화
	void Focus(int unit_id);		//스크롤을 유닛으로 자연스럽게 이동

	//세부 컨트롤(평상시/전투시)
	void NormalControl();
	void BattleControl();

public:
	//맵 로드
	bool Load(char* map_name, bool battle);

	//게임 흐름 제어
	void Control();		//키 입력
	void Process();		//게임 처리
	void Draw();		//화면 그리기

	//유닛 관련
	void SetHeroXY(int x, int y);	//주인공 좌표 지정
	void SetAutoHeroXY(int ev);		//자동 좌표 지정
	CUnit* GetUnit(int id);			//유닛 정보 얻기
	void SetActiveUnit(int id);		//활성화 유닛 고르기

	//저장, 불러오기
	void SaveGame(FILE* save_fp);
	void LoadGame(FILE* load_fp);

	//이벤트 관련
	void InitEvent();
	int GetEventNo();

	//길찾기 관련
	void SetStartPoint(int x, int y, int move_max);
	void SetEndPoint(int x, int y);
	void FindRoad(int x, int y, int expense, int move_max=99);
	bool MoveNextPointFast(int move);
	bool MoveNextPoint(int move);
	bool MoveNextPointStupid(int move, int iq);

	//생성자, 소멸자
	CSRPG();
	~CSRPG();
};
