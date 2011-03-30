//#include <vector>

//using namespace std;

//맵 한계
/*#define LAYER_MAX	3
#define MAP_XMAX	80
#define MAP_YMAX	60
#define TILESIZE	32
#define TILE_MAX	120

//유닛 정보
#define UNIT_MAX	128
*/
//유닛 방향
#define DIR_UP		0
#define DIR_LEFT	1
#define DIR_DOWN	2
#define DIR_RIGHT	3

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
	int x, y;		//좌표
	int px, py;		//출력 때의 좌표 조정을 위한 변수(x * TILESIZE + px 식으로 사용)

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