#pragma once

#include "GameProcess.h"

#define SCREEN_MAP_WIDTH	16
#define SCREEN_MAP_HEIGHT	12

class CDungeon : public CGameProcess
{
private:
	//맵 정보
	int map_id;
	int map_width;
	int map_height;

	//주인공 정보
	int hero_x;
	int hero_y;
	RECT hero_rect;

	//대화창
	CTextDlg m_dlg;

	bool HeroMove(int x, int y);	//주인공 이동 처리
	void HeroMoveEvent();			//주인공 이동 이벤트

public:
	void Control();
	void Process();
	void Render();

	CDungeon();
	~CDungeon();
};
