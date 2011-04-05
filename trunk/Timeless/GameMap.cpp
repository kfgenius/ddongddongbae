#include "gamemap.h"
#include "donglib.h"

///////////////////////////////////////////////////////////
// CGameMap 메소드

CGameMap::CGameMap(char* map_name, char* tile1_name, char* tile2_name)
{
}

CGameMap::~CGameMap()
{
}

//유닛 생성
void CGameMap::CreateUnit()
{
}

//맵을 미리 그리는 함수(매번 그리면 느리므로)
void CGameMap::DrawGround()
{
}

//스크롤 초기화
void CGameMap::InitScroll(int unit_id)
{
}

//포커스
void CGameMap::Focus(int unit_id)
{
}

//주인공 위치 설정
void CGameMap::SetHeroXY(int x, int y)
{
}

//주인공 위치를 이벤트 번호로 자동 배치
void CGameMap::SetAutoHeroXY(int ev)
{
}

CUnit* CGameMap::GetUnit(int id)
{
}

//해당 위치 유닛 찾기
int CGameMap::FindUnit(int x, int y)
{
}
