#include "gamemap.h"
#include "donglib.h"

///////////////////////////////////////////////////////////
// CGameMap �޼ҵ�

CGameMap::CGameMap(char* map_name, char* tile1_name, char* tile2_name)
{
}

CGameMap::~CGameMap()
{
}

//���� ����
void CGameMap::CreateUnit()
{
}

//���� �̸� �׸��� �Լ�(�Ź� �׸��� �����Ƿ�)
void CGameMap::DrawGround()
{
}

//��ũ�� �ʱ�ȭ
void CGameMap::InitScroll(int unit_id)
{
}

//��Ŀ��
void CGameMap::Focus(int unit_id)
{
}

//���ΰ� ��ġ ����
void CGameMap::SetHeroXY(int x, int y)
{
}

//���ΰ� ��ġ�� �̺�Ʈ ��ȣ�� �ڵ� ��ġ
void CGameMap::SetAutoHeroXY(int ev)
{
}

CUnit* CGameMap::GetUnit(int id)
{
}

//�ش� ��ġ ���� ã��
int CGameMap::FindUnit(int x, int y)
{
}
