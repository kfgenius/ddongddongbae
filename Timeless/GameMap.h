#include <stdio.h>

#include "Unit.h"
#include "Animation.h"

//맵 한계
#define LAYER_MAX			3
#define MAP_WIDTH_MAX		80
#define MAP_HEIGHT_MAX		60
#define TILE_SIZE			32
#define TILE_SOURCE_SIZE	48
#define TILE_TYPE_MAX		18

//유닛 한계
#define UNIT_TYPE_MAX		128

//주인공 ID
#define HERO	0

//////////////////////////////
//타일 정보
struct TileData
{
	unsigned char ground;	//바닥
	unsigned char object;	//위의 물건
	unsigned char unit;		//서 있는 유닛
	unsigned char event_no;	//이벤트
};

struct MapPoint
{
	int x;
	int y;
};

//////////////////////////////
//게임 클래스
class CGameMap
{
private:
	//맵 정보
	char map_name[80];
	unsigned char x_size, y_size;
	TileData map[MAP_WIDTH_MAX][MAP_HEIGHT_MAX];

	//타일 속성
	int tile_mov[2][TILE_TYPE_MAX];		//필요한 이동력
	int tile_attr[2][TILE_TYPE_MAX];		//속성
	int tile_damage[2][TILE_TYPE_MAX];	//대미지
	CUnitData unitdata[UNIT_TYPE_MAX];

	//애니메이션
	CAnimation ani;

	//스크롤 변수
	int scroll_x, scroll_y;
	int next_scroll_x, next_scroll_y;

	//유닛 정보
	CUnit unit[UNIT_TYPE_MAX];
	int unit_max;

	//유닛 관련
	void CreateUnit();			//유닛그림 생성(나중에 삭제)
	void DrawGround();			//맵 미리 그리기
	int FindUnit(int x, int y);		//해당위치의 유닛 찾기

	//스크롤 관련
	void InitScroll(int unit_id);	//스크롤을 유닛을 중심으로 초기화
	void Focus(int unit_id);		//스크롤을 유닛으로 자연스럽게 이동

public:
	//맵 출력
	void Draw1F();
	void Draw2F();
	void DrawNormalUnit();
	void DrawFlyUnit();

	//유닛 관련
	void SetHeroXY(int x, int y);	//주인공 좌표 지정
	void SetAutoHeroXY(int ev);		//자동 좌표 지정
	CUnit* GetUnit(int id);			//유닛 정보 얻기

	//생성자, 소멸자
	CGameMap(char* map_name, char* tile1_name, char* tile2_name);
	virtual ~CGameMap();
};
