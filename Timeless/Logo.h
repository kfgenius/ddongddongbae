#pragma once
#include "gameprocess.h"

class CLogo :
	public CGameProcess
{
private:
	int game_time;

public:
	void Control();
	void Process();
	void Render();

	CLogo(void);
	~CLogo(void);
};
