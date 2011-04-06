#pragma once
#include "gameprocess.h"

class CMainMenu : public CGameProcess
{
private:
	int game_time;
	char title_name[20];

public:
	void Process();

	CMainMenu(void);
	~CMainMenu(void);
};
