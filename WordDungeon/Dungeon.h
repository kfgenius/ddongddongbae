#pragma once

#include "GameProcess.h"

#define SCREEN_MAP_WIDTH	16
#define SCREEN_MAP_HEIGHT	12

class CDungeon : public CGameProcess
{
private:
	//�� ����
	int map_id;
	int map_width;
	int map_height;

	//���ΰ� ����
	int hero_x;
	int hero_y;
	RECT hero_rect;

	//��ȭâ
	CTextDlg m_dlg;

	bool HeroMove(int x, int y);	//���ΰ� �̵� ó��
	void HeroMoveEvent();			//���ΰ� �̵� �̺�Ʈ

public:
	void Control();
	void Process();
	void Render();

	CDungeon();
	~CDungeon();
};
