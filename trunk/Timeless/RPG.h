#pragma once

#include <stdio.h>

#include "Unit.h"
#include "GameProcess.h"

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

//이동 불가
#define MOVE_DISABLE	9999

//팀
enum
{
	party_ally,
	party_enemy,
	party_neutral
};

//////////////////////////////
//타일 정보
struct TileData
{
	unsigned char ground;	//바닥
	unsigned char object;	//위의 물건
	unsigned char unit;		//서 있는 유닛
	unsigned char event_no;	//이벤트

	//길 찾기용
	int move;		//소모되는 이동력
	int road;		//길을 검색하는 맵
	int distance;	//거리를 나타내는 맵
};

struct MapPoint
{
	int x;
	int y;
};

//////////////////////////////
//게임 클래스
class CRPG : public CGameProcess
{
private:
	//타일 종류 한계
	int tile_type_max;

	//이벤트 검사
	int Crush(int x, int y);		//이동시 이벤트 검사
	int Talk(int x, int y);			//대화시 이벤트 검사

	//조작
	void Control();

public:
	//맨 처음 페이드인
	BOOL first_fade_in;

	//스크롤 변수
	int scroll_x, scroll_y;
	int next_scroll_x, next_scroll_y;

	//이벤트
	int event_no;

	//길찾기 관련
	MapPoint point;		//지금 사물이 있는 위치

	//제어 관련
	int mode;		//모드
	int result;		//결과값

	//맵 정보
	char map_name[80];
	unsigned char x_size, y_size;
	TileData map[MAP_WIDTH_MAX][MAP_HEIGHT_MAX];

	//유닛 정보
	CUnit unit[UNIT_TYPE_MAX];
	int unit_max;
	int active_unit;	//활성화된 유닛

	//타일 속성
	int tile_mov[2][TILE_TYPE_MAX];		//필요한 이동력
	int tile_attr[2][TILE_TYPE_MAX];		//속성
	int tile_damage[2][TILE_TYPE_MAX];	//대미지
	CUnitData unitdata[UNIT_TYPE_MAX];

	//애니메이션
	CAnimation ani;

	//유닛 관련
	virtual void CreateUnit();			//유닛그림 생성(나중에 삭제)
	virtual void DrawGround();			//맵 미리 그리기
	virtual void UnitMove(int id, int dir);	//유닛 이동
	virtual int FindUnit(int x, int y);	//해당위치의 유닛 찾기

	//이동 관련
	virtual int	GetMoveSpeed(int x, int y);	//이동 가능성 검사

	//스크롤 관련
	virtual void InitScroll(int unit_id);	//스크롤을 유닛을 중심으로 초기화
	virtual void Focus(int unit_id);		//스크롤을 유닛으로 자연스럽게 이동

	//맵 출력
	virtual void Draw1F();
	virtual void Draw2F();
	virtual void Draw3F();

	//게임 흐름 제어
	virtual void Process();

	//유닛 관련
	virtual void SetHeroXY(int x, int y);	//주인공 좌표 지정
	virtual void SetAutoHeroXY(int ev);		//자동 좌표 지정
	virtual CUnit* GetUnit(int id);			//유닛 정보 얻기
	virtual void SetActiveUnit(int id);		//활성화 유닛 고르기

	//이벤트 관련
	virtual void InitEvent();
	virtual int GetEventNo();

	//생성자, 소멸자
	CRPG(char* map_name, char* tile1_name, char* tile2_name);
	virtual ~CRPG();
};